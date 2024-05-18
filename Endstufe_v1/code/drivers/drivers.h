#define BOARD_I2C i2c0
#define BOARD_I2C_SDA_PIN 0
#define BOARD_I2C_SCL_PIN 1

#define BOARD_UART uart1
#define BOARD_UART_TX_PIN 20
#define BOARD_UART_RX_PIN 21
#define BOARD_UART_BAUDRATE 115200

#define BOARD_SDZA_PIN 6
#define BOARD_FAULTZA_PIN 7
#define BOARD_SDZB_PIN 8
#define BOARD_FAULTZB_PIN 9

#define BOARD_XSMTA_PIN 4
#define BOARD_XSMTB_PIN 5


#define I2C_PCM1865_ADDRESS 0b1001010 // 0x94, LSB removed
#define I2C_PCM9211_ADDRESS 0x40 // 0x80, LSB removed
#define I2C_PCM5122a_ADDRESS 0b1001100 // 0x98, LSB removed
#define I2C_PCM5122b_ADDRESS 0b1001101// 0x9A, LSB removed

void pcm1865_init();
void pcm1865_printStatus();
void pcm9211_init();
void pcm9211_setSPDIF1();
void pcm5122_init();
void pcm5122_printStatus();
void pcm5122_powerdown();
void pcm5122_powerup();
void pcm5122_printReadOnly();
void tpa3116d2_init();