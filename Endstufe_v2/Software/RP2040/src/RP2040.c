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
    read_dsp_register(0xF4, 0x05, 2);

    printf("More DSP Status: ");
    read_dsp_register(0xF4, 0x00, 10);
    
    printf("Panic Register: ");
    read_dsp_register(0xF4, 0x28, 2);

    printf("MP23: ");
    read_dsp_register(0xF5, 0xC9, 2);
}

void reset_dsp() {
    printf("Resetting DSP\n");
    gpio_init(BOARD_ADAU1467_RST_Pin);
    gpio_set_dir(BOARD_ADAU1467_RST_Pin, GPIO_OUT);
    gpio_put(BOARD_ADAU1467_RST_Pin, 0);
    sleep_ms(100);
    gpio_put(BOARD_ADAU1467_RST_Pin, 1);
    sleep_ms(100);
}

void setup_dsp() {
    reset_dsp();
    load_sigmastudio_program_adau1467();
    clear_adau1467_panic();
    sleep_ms(10);
}

void pcm1865_init() {
    uint8_t data[2] = {0, 0};

    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // Enable Automatic Clipping Supression
    data[0] = 0x05;
    data[1] = 0b10111111;
    //status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

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
    // XTAL clock source, Master mode, Auto Clock Detector 
    data[0] = 0x20;
    data[1] = 0b10010001;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    // Set 12.288 BCLK
    data[0] = 0x26;
    data[1] = 0b00000001;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);

    // BCLK is devided by 64 in 0x27 which makes 192kHz LRCK


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
    uint8_t data[10];
    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // 0x72 should be 0x0F -> Run state
    data[0] = 0x70;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, true);
    printf("%d\n", status);
    // Now the PCM1685 knows where to start reading from, we can read now
    status = i2c_read_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 10, false);
    printf("%d\n", status);

    // Print out the data
    for(int i = 0; i < 10; i++)
        printf("0x%x\n", data[i]);
}

void pcm1865_printGain() {
    uint8_t data[10];
    // Select Page 0
    data[0] = 0x00;
    data[1] = 0x00;
    int status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 2, false);
    printf("%d\n", status);

    data[0] = 0x01;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, true);
    printf("%d\n", status);
    // Now the PCM1685 knows where to start reading from, we can read now
    status = i2c_read_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 4, false);
    printf("%d\n", status);

    printf("Gain settings of PCM1865:\n");
    // Print out the data
    for(int i = 0; i < 4; i++)
        printf("0x%x\n", data[i]);

    data[0] = 0x0F;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, true);
    printf("%d\n", status);
    // Now the PCM1685 knows where to start reading from, we can read now
    status = i2c_read_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, false);
    printf("%d\n", status);

    // Print out the data
    for(int i = 0; i < 1; i++)
        printf("0x%x\n", data[i]);

    data[0] = 0x16;
    status = i2c_write_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 1, true);
    printf("%d\n", status);
    // Now the PCM1685 knows where to start reading from, we can read now
    status = i2c_read_blocking(BOARD_I2C, I2C_PCM1865_ADDRESS, data, 3, false);
    printf("%d\n", status);

    // Print out the data
    for(int i = 0; i < 3; i++)
        printf("0x%x\n", data[i]);
}

void shutdown_amplifiers() {
    printf("Disabling amplifiers!\n");

    gpio_put(BOARD_MUTEA_PIN, 1);
    gpio_put(BOARD_MUTEB_PIN, 1);
    gpio_put(BOARD_SDZA_PIN, 0);
    gpio_put(BOARD_SDZB_PIN, 0);
}

void enable_amplifiers() {
    printf("Enabling amplifiers!\n");
    
    gpio_put(BOARD_MUTEA_PIN, 0);
    gpio_put(BOARD_MUTEB_PIN, 0);
    gpio_put(BOARD_SDZA_PIN, 1);
    gpio_put(BOARD_SDZB_PIN, 1);
}

void setup_adau1962a() {
    uint8_t data[2] = {0, 0};

    // XTALI clock, xtal enabled, normal operation, Setting 12 for 24.576 SCK in 192kHz mode
    data[0] = 0x00;
    data[1] = 0b00000001;
    int status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    sleep_ms(50);

    // XTALI clock, xtal enabled, normal operation, Setting 12 for 24.576 SCK in 192kHz mode
    data[0] = 0x00;
    data[1] = 0b00000101;
    status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // I2S, TDM4, 192Khz Low Propagation Delay, Stay muted
    data[0] = 0x06;
    data[1] = 0b00010101;
    status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // Latch on BCLK Falling Edge
    data[0] = 0x07;
    data[1] = 0b00000010;
    status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    // Make outputs quieter
    data[0] = 0x0B;
    data[1] = 0b00001000;
    status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    sleep_ms(50);

    // Unmute
    data[0] = 0x06;
    data[1] = 0b00010100;
    status = i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);
    printf("%d\n", status);

    sleep_ms(50);

    // https://ez.analog.com/audio/f/q-a/90292/adau1966a-register-programming-sequence/264515
}

void read_i2c_register(const uint8_t i2c_addr, const uint8_t reg_addr, uint8_t length) {
    uint8_t dst[36];
    i2c_write_blocking(BOARD_I2C, i2c_addr, &reg_addr, 1, true);
    int bytes_read = i2c_read_blocking(BOARD_I2C, i2c_addr, dst, length, false);
    printf("Read %d bytes from i2c_addr 0x%x, register 0x%x: ", bytes_read, i2c_addr, reg_addr);
    for(int i = 0; i < length; i++) {
        printf("0x%x ", dst[i]);
    }
    printf("\n");
}

void read_adau1962a() {
    printf("ADAU1962A register dump:\n");
    read_i2c_register(I2C_ADAU1962A_ADDRESS, 0x00, 32);
    //read_i2c_register(I2C_ADAU1962A_ADDRESS, 0x06, 1);
}

void unmute_adau1962a() {
    uint8_t data[2] = {0, 0};
    // Unmute
    data[0] = 0x06;
    data[1] = 0x04;
    i2c_write_blocking(BOARD_I2C, I2C_ADAU1962A_ADDRESS, data, 2, false);

}

void pretty_register_dump() {
    printf("\n--- Beginning Pretty Register Dump ---\n");

    // Konfiguration der ICs mit ihren Adressbereichen
    struct {
        uint8_t address;  // I2C-Adresse
        uint8_t start_reg; // Startregister
        uint8_t end_reg;   // Endregister
    } i2c_devices[] = {
        {I2C_PCM1865_ADDRESS, 0x00, 0x78},    // PCM1865
        {I2C_ADAU1962A_ADDRESS, 0x00, 0x1F}, // ADAU1962A
        // {I2C_ADAU1467_ADDRESS, 0x00, 0xFF},  // ADAU1467 (Beispielbereich)
    };

    // Anzahl der Geräte ermitteln
    size_t device_count = sizeof(i2c_devices) / sizeof(i2c_devices[0]);

    // Für jedes Gerät die Register ausgeben
    for (size_t i = 0; i < device_count; i++) {
        uint8_t address = i2c_devices[i].address;
        uint8_t start_reg = i2c_devices[i].start_reg;
        uint8_t end_reg = i2c_devices[i].end_reg;

        printf("Device 0x%X - Registers 0x%X to 0x%X:\n", address, start_reg, end_reg);
        for (uint8_t reg = start_reg; reg <= end_reg; reg++) {
            uint8_t reg_address = reg;
            uint8_t data = 0;

            // Schreibe das Register, das gelesen werden soll
            if (i2c_write_blocking(BOARD_I2C, address, &reg_address, 1, true) <= 0) {
                printf("  Failed to write to device 0x%X at register 0x%X\n", address, reg);
                continue;
            }

            // Lese die Daten vom Register
            if (i2c_read_blocking(BOARD_I2C, address, &data, 1, false) <= 0) {
                printf("  Failed to read from device 0x%X at register 0x%X\n", address, reg);
                continue;
            }

            printf("  Register 0x%02X: 0x%02X\n", reg, data);
        }
        printf("\n");
    }

    printf("\n--- End of Pretty Register Dump ---\n");
}

typedef struct {
    uint8_t pin;       // GPIO pin number
    uint8_t direction; // GPIO_IN or GPIO_OUT
} gpio_config_t;

const gpio_config_t gpio_configs[I2C_SLAVE_BUFFER_SIZE] = {
    {BOARD_SDZA_PIN, GPIO_OUT},
    {BOARD_FAULTZA_PIN, GPIO_IN},
    {BOARD_SDZB_PIN, GPIO_OUT},
    {BOARD_FAULTZB_PIN, GPIO_IN},
    {BOARD_MUTEA_PIN, GPIO_OUT},
    {BOARD_MUTEB_PIN, GPIO_OUT},
    {BOARD_PCM9211_RST_PIN, GPIO_OUT},
    {BOARD_ADAU1467_RST_Pin, GPIO_OUT},
    {BOARD_ADAU1962A_RST_Pin, GPIO_OUT},
};

uint8_t previous_buffer[I2C_SLAVE_BUFFER_SIZE] = {0};

void update_gpios_on_change(const uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        const gpio_config_t *config = &gpio_configs[i];
        if (config->direction == GPIO_IN) {
            continue; // Skip inputs
        }

        if (buffer[i] != previous_buffer[i]) {
            gpio_put(config->pin, buffer[i]); // Update GPIO
            previous_buffer[i] = buffer[i];  // Save new state
        }
    }
}

void read_input_gpios(uint8_t *status_buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        const gpio_config_t *config = &gpio_configs[i];

        if (config->direction == GPIO_IN) {
            uint8_t current_state = gpio_get(config->pin); // Read current input state

            // Only update status_buffer if the state has changed
            if (current_state != (status_buffer[i] & 0x01)) {
                status_buffer[i] = current_state;  // Update buffer with new state
            }
        }
    }
}

void initialize_gpios() {
    for (size_t i = 0; i < I2C_SLAVE_BUFFER_SIZE; i++) {
        const gpio_config_t *config = &gpio_configs[i];
        gpio_init(config->pin);
        gpio_set_dir(config->pin, config->direction);

        if (config->direction == GPIO_OUT) {
            gpio_put(config->pin, 0); // Default output state
        }
    }
}

int main()
{
    // Configure and enable UART
    stdio_uart_init_full(BOARD_UART, BOARD_UART_BAUDRATE, BOARD_UART_TX_PIN, BOARD_UART_RX_PIN);

    // Setup amp gpios
    initialize_gpios();

    setup_i2c_slave();

    printf("\nEndstufe\n");

    // testFunc();

    // shutdown_amplifiers();

    // setup_dsp();

    // enable_adau1962a();
    // enable_amplifiers();

    // context.mem[0] = 1;
    // context.mem[2] = 1;
    // context.mem[7] = 1;
    // context.mem[8] = 1;

    while(true) {

        update_gpios_on_change(context.mem, I2C_SLAVE_BUFFER_SIZE);
        read_input_gpios(context.mem, I2C_SLAVE_BUFFER_SIZE);

        sleep_ms(1);
        // pretty_register_dump();
    }

    
    setup_dsp();

    // dsp_status();

    pcm1865_init();
    sleep_ms(100);
    // pcm1865_printStatus();

    enable_adau1962a();
    sleep_ms(10);
    load_sigmastudio_program_adau1962a();
    sleep_ms(100);
    //setup_adau1962a();
    // read_adau1962a();
    sleep_ms(10);
    unmute_adau1962a();

    //pcm1865_printGain();

    pretty_register_dump();

    while (true) {
        sleep_ms(1000);
        enable_amplifiers();
        sleep_ms(1000);
        //shutdown_amplifiers();
    }
}
