#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "drivers/drivers.h"

int main() {
    // Enable UART so we can print status output

    stdio_uart_init_full(BOARD_UART, BOARD_UART_BAUDRATE, BOARD_UART_TX_PIN, BOARD_UART_RX_PIN);
    // stdio_init_all();
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(BOARD_I2C, 100 * 1000);
    gpio_set_function(BOARD_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BOARD_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(BOARD_I2C_SDA_PIN);
    gpio_pull_up(BOARD_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    printf("\nEndstufe\n");

    pcm1865_init();
    sleep_ms(100);
    pcm1865_printStatus();
    pcm9211_init();
    sleep_ms(100);
    pcm9211_setSPDIF1();
    sleep_ms(100);

    pcm5122_init();
    sleep_ms(1000);
    pcm5122_printReadOnly();
    pcm5122_printStatus();
    
    tpa3116d2_init();


    printf("Waiting forever\n");

    while (1);

    printf("Exiting...\n");
    return 0;
}