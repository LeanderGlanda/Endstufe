#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include <string.h>

#include "definitions.h"
#include "dsp/ADAU1467.h"

void enable_adau1962a() {
    printf("Enabling ADAU1962a\n");
    gpio_init(BOARD_ADAU1962A_RST_Pin);
    gpio_set_dir(BOARD_ADAU1962A_RST_Pin, GPIO_OUT);
    gpio_put(BOARD_ADAU1962A_RST_Pin, 1);
}

void read_dsp_register(uint8_t addrhigh, uint8_t addrlow, uint8_t length) {
    uint8_t dst[10];
    uint8_t register_address_bytes[2] = { addrhigh, addrlow};
    i2c_write_blocking(BOARD_I2C, I2C_ADAU1467_ADDRESS, register_address_bytes, 2, true);
    int bytes_read = i2c_read_blocking(BOARD_I2C, I2C_ADAU1467_ADDRESS, dst, length, false);
    printf("Bytes read: %d\n", bytes_read);
    for(int i = 0; i < length; i++) {
        printf("%x\n", dst[i]);
    }
}

void write_dsp_register(uint8_t addrhigh, uint8_t addrlow, uint8_t length, uint8_t* data) {
    uint8_t* write_buffer = (uint8_t*)malloc(length + 2);
    write_buffer[0] = addrhigh;
    write_buffer[1] = addrlow;
    memcpy(&write_buffer[2], data, length);
    int bytes_written = 0;
    bytes_written += i2c_write_blocking(BOARD_I2C, I2C_ADAU1467_ADDRESS, write_buffer, length + 2, false);
    printf("Bytes written: %d\n", bytes_written);
    free(write_buffer);
}

void write_mp23_pullup() {
    uint8_t content[2];
    content[0] = 0x00;
    content[1] = 0x05;
    write_dsp_register(0xF5, 0xC9, 2, content);
}

void clear_adau1467_panic() {
    uint8_t content[2];
    content[0] = 0x00;
    content[1] = 0x01;
    write_dsp_register(0xF4, 0x21, 2, content);
    content[1] = 0x00;
    write_dsp_register(0xF4, 0x21, 2, content);
}

void dsp_status() {
    printf("DSP Status: ");
    read_dsp_register(0xF4, 0x05, 2); // DSP Status

    printf("More DSP Status: ");
    read_dsp_register(0xF4, 0x00, 10); // DSP Status
    
    printf("Panic Register: ");
    read_dsp_register(0xF4, 0x28, 2); // Panic register

    // write_mp23_pullup();

    // uint8_t content[2];
    // content[0] = 0x00;
    // content[1] = 0x01;
    // write_dsp_register(0xF5, 0xd9, 2, content);

    printf("MP23: ");
    read_dsp_register(0xF5, 0xC9, 2); // MP23 register
}

void reset_dsp() {
    printf("Resetting DSP\n");
    gpio_init(BOARD_ADAU1467_RST_Pin);
    gpio_set_dir(BOARD_ADAU1467_RST_Pin, GPIO_OUT);
    gpio_put(BOARD_ADAU1467_RST_Pin, 0);
    sleep_ms(100);
    gpio_put(BOARD_ADAU1467_RST_Pin, 1);
}

int main()
{
    // Enable UART so we can print status output

    stdio_uart_init_full(BOARD_UART, BOARD_UART_BAUDRATE, BOARD_UART_TX_PIN, BOARD_UART_RX_PIN);
    // stdio_init_all();
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(BOARD_I2C, 100 * 1000);
    gpio_set_function(BOARD_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BOARD_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(BOARD_I2C_SDA_PIN);
    gpio_pull_up(BOARD_I2C_SCL_PIN);

    printf("\nEndstufe\n");

    enable_adau1962a();
    
    reset_dsp();
    //read_dsp_register(0xF4, 0x28, 2); // Panic register
    //clear_adau1467_panic();
    //read_dsp_register(0xF4, 0x28, 2); // Panic register

    // dsp_status();

    setup_adau1467();

    //read_dsp_register(0xF4, 0x28, 2); // Panic register
    //clear_adau1467_panic();
    //read_dsp_register(0xF4, 0x28, 2); // Panic register

    sleep_ms(500);
    dsp_status();

    while (true) {
        // printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
