// 06.04.2025 Bart3kTK

#ifndef AD7190_H
#define AD7190_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define AD7190_DEV_ID   0x00 //TODO: Trzeba zobacvzc ale adres to 0xX4
#define AD7190_DEV_RESET 0xFFFFFFFFFF // reset value

/* AD7190 communication register */
#define AD7190_COMM_WEN      (1 << 7)
#define AD7190_COMM_R        (1 << 6)
#define AD7190_COMM_W        (0 << 6)
#define AD7190_COMM_ADDR(x)  ((x & 0x7) << 3)
#define AD7190_COMM_CREAD    (1 << 2)


/* AD7190 Register Map              */
/* AD7190_COMM_ADDR(x) - variants   */
#define AD7190_REG_COMMUNICATIONS   0 // 8 bit WO
#define AD7190_REG_STATUS           0 // 8 bit RO
#define AD7190_REG_MODE             1 // 24-bit RW
#define AD7190_REG_CONFIGURATION    2 // 24-bit RW
#define AD7190_REG_DATA             3 // 24/32-bit RO
#define AD7190_REG_ID               4 // 8-bit RO
#define AD7190_REG_GPOCON           5 // 8-bit RW
#define AD7190_REG_OFFSET           6 // 24-bit RW
#define AD7190_REG_FULLSCALE        7 // 24-bit RW

/* AD7190 mode register */
#define AD7190_MODE_DEFAULT        0x080060 // Default mode
#define AD7190_MODE_SEL(x)         (((x) & 0x7) << 21)
#define AD7190_MODE_DATA_STA       (1 << 20)
#define AD7190_MODE_CLK_INT        (2 << 18)
#define AD7190_MODE_FILTER(x)      ((x) & 0x0A)



/* AD7190 mode register modes       */
/* AD7190_MODE_SEL(x) - variants    */
#define AD7190_MODE_CONTINUOUS      0 // Continuous conversion mode
#define AD7190_MODE_IDLE            2 // Idle mode
#define AD7190_MODE_ZERO_SCALE_CAL  4 // Zero scale calibration mode
#define AD7190_MODE_FULL_SCALE_CAL  5 // Full scale calibration mode

/* AD7190 mode register filter rates */
/* AD7190_MODE_FILTER(x) - variants    */
#define AD7190_FILTER_WORD_16       16 // 75 Hz output data rate (wchen using sinc4 filter and chop is enabled)
#define AD7190_FILTER_WORD_8        8  // 150 Hz output data rate (wchen using sinc4 filter and chop is enabled)

/* ADC7190 configuration register */
#define AD7190_CONFIG_DEFAULT       0x000117    // Default configuration
#define AD7190_CONFIG_CHOP          (1 << 23)   // CHOP enable.
#define AD7190_CONFIG_BUF           (1 << 4)    // Buffered Mode Enable.
#define AD7190_CH_AIN1P_AIN2M       (1 << 8)    // AIN1(+) - AIN2(-) 
#define AD7190_CONFIG_GAIN_128      (7 << 0)    // Gain 128  


typedef bool (*ad7190_SPI_read)(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len);
typedef bool (*ad7190_SPI_write)(uint8_t* tx_data, size_t tx_len);
typedef void (*ad7190_delay)(uint32_t millis);

typedef struct 
{
    uint8_t dev_addr; 
    ad7190_SPI_read read_func;
    ad7190_SPI_write write_func;
    ad7190_delay delay_func;
} ad7190_t;

typedef struct{
    uint8_t status_register;
    uint32_t raw_data;
} ad7190_data_t;

bool _ad7190_init(ad7190_t* dev);
bool _ad7190_check_id(ad7190_t* dev);
bool _ad7190_calibrate(ad7190_t* dev);
bool _ad7190_reset(ad7190_t* dev);
bool _ad7190_get_data(ad7190_t* dev, ad7190_data_t *data);
bool _ad7190_set_continuous_mode(ad7190_t* dev);
bool _ad7190_setup(ad7190_t* dev);

bool _ad7190_write_data(uint8_t* tx_data, size_t tx_len);
bool _ad7190_read_data(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len);

#endif // AD7190_H