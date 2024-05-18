#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "drivers.h"


static char event_str[128];

void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    printf("GPIO %d %s\n", gpio, event_str);
}

void tpa3116d2_init() {

    printf("Enabling interrupts for FAULTZ/B\n");
    gpio_set_irq_enabled_with_callback(BOARD_FAULTZA_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BOARD_FAULTZB_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    printf("Enabling speaker outputs\n");
    gpio_init(BOARD_SDZA_PIN);
    gpio_init(BOARD_SDZB_PIN);
    gpio_set_dir(BOARD_SDZA_PIN, GPIO_OUT);
    gpio_set_dir(BOARD_SDZB_PIN, GPIO_OUT);
    gpio_put(BOARD_SDZA_PIN, 0);
    gpio_put(BOARD_SDZB_PIN, 0);
}


static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}