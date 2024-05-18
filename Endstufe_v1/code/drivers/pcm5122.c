#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "drivers.h"

/*
New day new plan: Maybe I'll try to configure PLL manually.
First thing noticed: PLL only works up with clocks up to 20Mhz, so input needs to be BCK
Is auto clock configuration even set? In 0x25 page 0
Actually PLL supports SCK up to 50Mhz?
Maybe the 24.576Mhz is just not supported for auto configure... 
Generating from SCK has the advantage that no BCK noise comes into parts like the DAC (datasheet states)
Let's use SCK with 24.576Mhz for internal clock generation.
Clock source needs to be configured in Page 0, Register 13
OSRCK must be set to 16fs by DOSR Register 30 -> 192kHz * 16 = 3,072Mhz
PLL needs to be turned on in Register 4

Values for registers:

*/

void configure_manual_clocks() {
    uint8_t data[2] = {0, 0};

    // All configured values are recommendations from the datasheet for SCK = 24.576Mhz and fs = 192kHz

    // Enable PLL
    data[0] = 0x04;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // DAC source clock should come from PLL clock
    data[0] = 0x0E;
    data[1] = 0b00010000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // PLL P = 2
    data[0] = 0x14;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // PLL J = 8
    data[0] = 0x15;
    data[1] = 0b00001000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // PLL D = 0
    data[0] = 0x16;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);
    data[0] = 0x17;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // PLL R = 1
    data[0] = 0x18;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // DSP Clock Divider = Divide by 2
    data[0] = 0x1B;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // DAC Clock Divider = Divide by 16
    data[0] = 0x1C;
    data[1] = 0b00001111;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // NCP Clock Divider = Divide by 4
    data[0] = 0x1C;
    data[1] = 0b00000011;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // OSR Clock Divider = Divide by 2
    data[0] = 0x1E;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // Set FS Speed mode to Quad speed 192kHz
    // Not enabeling 16x interpolation
    data[0] = 0x22;
    data[1] = 0b00000010;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // Automatic clock detection is enabled (default)
    // clock errors/halts are regarded for errors
    data[0] = 0x25;
    data[1] = 0b00000010;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // No setting for Register 32 & 33 which would be used for generating LRCK and BCK in master mode
    // Register 35 & 36 are correct with 256 DSP clock cycles are available in one audio frmae

}

void pcm5122_init() {

    printf("Soft Un-Mute\n");
    gpio_init(BOARD_XSMTA_PIN);
    gpio_init(BOARD_XSMTB_PIN);
    gpio_set_dir(BOARD_XSMTA_PIN, GPIO_OUT);
    gpio_set_dir(BOARD_XSMTB_PIN, GPIO_OUT);
    gpio_put(BOARD_XSMTA_PIN, 1);
    gpio_put(BOARD_XSMTB_PIN, 1);

    uint8_t data[2] = {0, 0};

    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // Disable PLL (auto clock configure)
    data[0] = 0x04;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // 0x07 SDOUT is selected to DSP output by default

    // Set GPIO6 as output (for SDOUT)
    data[0] = 0x08;
    data[1] = 0b00100000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // 0x09 BCK and LRCK are inputs by default
    // 0x0D PLL reference is set to SCK by default

    // DAC source clock should come from Master clock (default)
    data[0] = 0x0E;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);
    
    // FS speed is set by clock automatically so no changes needed.
    // Not enabeling 16x interpolation
    data[0] = 0x22;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // Automatic clock detection is enabled (default)
    // clock errors/halts are regarded for errors
    data[0] = 0x25;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // 0x28 I2S format is default data format with 24 bit word length
    // 0x2A left/right channel control (which is which)
    
    // Select fixed process flow
    data[0] = 0x2B;
    data[1] = 0b00000101;
    //i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // 0x3B Auto mute time is set to 21ms right and left channel
    
    // Digital volume control: right follows left channel
    data[0] = 0x3C;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // Set left digital volume to 0.0dB
    // right channel should follow this
    data[0] = 0x3D;
    // data[1] = 0b00110000;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    // 0x3F ramp down is enabled by default
    // 0x41 auto mute enabled + R+L at the same time

    // Set GPIO6 output to SDOUT
    data[0] = 0x55;
    data[1] = 0b00000111;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);


    // Page 1
    // 0x02 Analog Gain for both channels is 0dB
    // 0x06 Analog Mute control follows digital mute. does it disable it again?
    // 0x09 VCOM is powered down -> correct for this design

    // lets hope all other coefficents are correct enough to be tested out...
}

void pcm5122_printReadOnly() {
    uint8_t data[2];
    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    for (int i = 0; i < 30; i++)
    {
        data[0] = 0x5B + i; // Detected FS and SCK Ratio
        i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 1, true);
        i2c_read_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 1, false);
        printf("0x%x: 0x%x\n", i + 0x5B, data[0]);
    }
} 

void pcm5122_printStatus() {
    uint8_t data[2];
    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    data[0] = 0x76; // Detected FS and SCK Ratio
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 1, true);
    i2c_read_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 1, false);

    printf("0x76: 0x%x\n", data[0]);
} 

void pcm5122_powerdown() {
    uint8_t data[2] = {0, 0};

    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    data[0] = 0x02;
    data[1] = 0b00000001;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);
}

void pcm5122_powerup() {
    uint8_t data[2] = {0, 0};

    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);

    data[0] = 0x02;
    data[1] = 0b00000000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM5122a_ADDRESS, data, 2, false);
}