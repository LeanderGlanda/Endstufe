#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "drivers.h"

/*
PCM9211 should input 192kHz from PCM1865
-> fs = 192kHz
-> BCK = 64fs = 12.288 Mhz
-> SCKI = 128fs = 24.576 Mhz

PCM9211 should output 192kHz signal to PCM5211
-> fs = 192kHz
-> BCK = 64fs = 12.288 Mhz
-> SCKO = 128fs = 24.576 Mhz
=> OK for PCM5211 as input and clock source
*/

void pcm9211_init() {
    uint8_t data[2] = {0, 0};
    // Maybe 0x24 needs to be changed with MCKO not enabled
    // 0x2F: DIR output data format setting is 24Bit MSB first I2S
    // 0x30: Clock frequency 256fs is set and should be correct
    

    // Change clock speeds BCK, SCK, LRCK
    // See top for description
    data[0] = 0x31;
    data[1] = 0b00000000; // 24.576 Mhz XTI, 12.288Mhz BCK, 192kHz LRCK
    i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);

    // 0x32 DIR Clock source configuration

    // Change clock speeds SBCK, SLRCK
    data[0] = 0x33;
    data[1] = 0b00000000; // 24.576 Mhz XTI, 12.288Mhz BCK, 192kHz LRCK
    i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);

    // Select DIR input (either S/PDIF 1 or 2)                          important
    // data[0] = 0x34;
    // data[1] = 0b11000010; 
    // i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);

    // Change main output port clock to AUXIN0 AND BCK,LRCK,Data to PCM1865 output
    data[0] = 0x6B;
    data[1] = 0b00000011;
    i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);

    // 6D: Main Output Port is Output not Hi-Z

    // word length should be set to 24 Bit
 
}

void pcm9211_setSPDIF1() {
    uint8_t data[2] = {0, 0};

    // Select DIR input (S/PDIF 1)
    data[0] = 0x34;
    data[1] = 0b11000010; 
    i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);

    // Change main output port clock to DIR AND BCK,LRCK,Data to PCM1865 output
    data[0] = 0x6B;
    data[1] = 0b00010000;
    i2c_write_blocking(BOARD_I2C, I2C_PCM9211_ADDRESS, data, 2, false);
}

void pcm9211_printStatus() {
    
}