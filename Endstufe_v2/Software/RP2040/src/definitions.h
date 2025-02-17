#define BOARD_I2C i2c0
#define BOARD_I2C_SDA_PIN 0
#define BOARD_I2C_SCL_PIN 1

#define BOARD_UART uart1
#define BOARD_UART_TX_PIN 20
#define BOARD_UART_RX_PIN 21
#define BOARD_UART_BAUDRATE 115200

#define BOARD_SDZA_PIN 2
#define BOARD_FAULTZA_PIN 4
#define BOARD_SDZB_PIN 3
#define BOARD_FAULTZB_PIN 5
#define BOARD_MUTEA_PIN 6
#define BOARD_MUTEB_PIN 7
#define BOARD_PCM9211_RST_PIN 8
#define BOARD_ADAU1467_RST_Pin 9
#define BOARD_ADAU1962A_RST_Pin 10

#define I2C_PCM1865_ADDRESS 0b1001010 // 0x94, LSB removed
#define I2C_PCM9211_ADDRESS 0x40 // 0x80, LSB removed
#define I2C_ADAU1467_ADDRESS 0x38
#define I2C_ADAU1962A_ADDRESS 0x04
#define I2C_RP2040_ADDRESS 0x42

void setup_i2c_slave();

typedef struct {
    uint8_t mem[256];
    uint8_t mem_address;
    bool mem_address_written;
} context_t;

extern context_t context; 

#define I2C_SLAVE_BUFFER_SIZE 9