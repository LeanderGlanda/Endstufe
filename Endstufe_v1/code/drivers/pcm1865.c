#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "drivers.h"

void pcm1865_init() {
    uint8_t data[2] = {0, 0};

    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // Enable Automatic Clipping Supression
    data[0] = 0x05;
    data[1] = 0b10000111;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    // Settings for ADC1 OK (0x06 and 0x07)
    // ADC2 needs to be configured for VIN(L/R)2
    // Polarity for both is not inverted
    data[0] = 0x08;
    data[1] = 0b01000010;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    data[0] = 0x09;
    data[1] = 0b01000010;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    
    // Configure SCK Clock source and Master Mode
    data[0] = 0x20;
    data[1] = 0b01010001;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    // The following register should be configured correctly:
    // 0x0B: One can change I2S to other formats and change the word length. (24 Bit default)
    // 0x0C-0x0E: TDM stuff
    // 0x0F: DPGA CH1 Left Gain Setting
    // 0x10-0x15: GPIO config / status
    // 0x16-0x19: DPGA (Gain) Settings
    // 0x1A-0x1B: Mic/DIN Stuff
    // 0x21-0x2D DSP/ADC Clock Divider Values
    // 0x30-0x57 Interrupt / Signal Detection sleep / etc. (Also scan time in sleep) / DC level
    // 0x58-62 AUXADC stuff and interrupt / control stuff
    // 0x70 Enter Sleep states
    // 0x71 Mute Control and 2 channels / 4 channels selection    
}

void pcm1865_printStatus() {
    uint8_t data[8];
    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // 0x72 should be 0x0F -> Run state
    data[0] = 0x72;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, true);
    printf("%d\n", status);
    // Now the PCM1685 knows where to start reading from, we can read now
    status = i2c_read_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 8, false);
    printf("%d\n", status);

    // Print out the data
    for(int i = 0; i < 8; i++)
        printf("0x%x\n", data[i]);
}