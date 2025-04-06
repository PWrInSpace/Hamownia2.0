// 06.04.2025 Bart3kTK

#include "ad7190.h"
#include "mcu_spi_config.h"

bool _ad7190_init(ad7190_t* dev)
{
    if(dev->write_func == NULL || dev->read_func == NULL)
    {
        return false;
    }

    if(dev->dev_addr != AD7190_DEV_ID)
    {
        return false;
    }

    if(_ad7190_reset(dev) == false)
    {
        return false;
    }
    // if(_ad7190_check_id(dev) == false)  TODO: nie wiem jaki ma id xd
    // {
    //     return false;
    // }
    if(_ad7190_calibrate(dev) == false)
    {
        return false;
    }
    if(_ad7190_setup(dev) == false)
    {
        return false;
    }


    return true;
}

bool _ad7190_setup(ad7190_t* dev)
{
    /* This setup is setting chop so*/
    /* make sure that calibration is done */
    uint8_t header = 0;
    header |= AD7190_COMM_W;
    header |= AD7190_COMM_ADDR(AD7190_REG_MODE);

    uint32_t data = (header << 24);
    data |= AD7190_MODE_DEFAULT;
    data |= AD7190_MODE_SEL(AD7190_MODE_IDLE);
    data |= AD7190_MODE_CLK_INT;
    data |= AD7190_MODE_DATA_STA;
    data |= AD7190_MODE_FILTER(AD7190_FILTER_WORD_8);

    uint8_t tx_data[4];
    tx_data[0] = (data >> 24) & 0xFF;
    tx_data[1] = (data >> 16) & 0xFF;
    tx_data[2] = (data >> 8) & 0xFF;
    tx_data[3] = data & 0xFF;

    if(dev->write_func(tx_data, 4) == false)
    {
        return false;
    }

    dev->delay_func(10); // 10 ms delay for setup (in data sheet 5 ms, but 10 ms is more stable)

    header = 0;
    header |= AD7190_COMM_W;
    header |= AD7190_COMM_ADDR(AD7190_REG_CONFIGURATION);

    data = (header << 24);
    data |= AD7190_CONFIG_DEFAULT;
    data |= AD7190_CONFIG_CHOP;
    data |= AD7190_CONFIG_BUF;
    data |= AD7190_CH_AIN1P_AIN2M;

    tx_data[0] = (data >> 24) & 0xFF;
    tx_data[1] = (data >> 16) & 0xFF;
    tx_data[2] = (data >> 8) & 0xFF;
    tx_data[3] = data & 0xFF;
    if(dev->write_func(tx_data, 4) == false)
    {
        return false;
    }
    dev->delay_func(10); // 10 ms delay for setup (in data sheet 5 ms, but 10 ms is more stable)

    return true;
}

bool _ad7190_reset(ad7190_t* dev)
{
    uint64_t data = AD7190_DEV_RESET;
    uint8_t tx_data[5];
    tx_data[0] = (data >> 32) & 0xFF;
    tx_data[1] = (data >> 24) & 0xFF;
    tx_data[2] = (data >> 16) & 0xFF;
    tx_data[3] = (data >> 8) & 0xFF;
    tx_data[4] = data & 0xFF;
    if(dev->write_func(tx_data, 5) == false)
    {
        return false;
    }
    dev->delay_func(10); // 10 ms delay for reset (in data sheet 0.5 ms, but 10 ms is more stable)
    return true;
}
bool _ad7190_check_id(ad7190_t* dev)
{

    uint8_t header = 0;
    header |= AD7190_COMM_R;
    header |= AD7190_COMM_ADDR(AD7190_REG_ID);

    uint8_t tx_data[1];
    tx_data[0] = header;

    uint8_t rx_data[1];

    if(dev->read_func(tx_data, 1, rx_data, 1) == false)
    {
        return false;
    }
    if(rx_data[0] != AD7190_DEV_ID)
    {
        return false;
    }
    return true;

}

bool _ad7190_calibrate(ad7190_t* dev)
{
    /* Calibration should be done when chop is disabled */
    /* Calibration is needed only when the gain is changed */
    /* Default gain is 128 */

    uint8_t header = 0;
    header |= AD7190_COMM_W;
    header |= AD7190_COMM_ADDR(AD7190_REG_MODE);

    uint32_t data = (header << 24);
    data |= AD7190_MODE_DEFAULT;
    data |= AD7190_MODE_SEL(AD7190_MODE_ZERO_SCALE_CAL);

    uint8_t tx_data[4];
    tx_data[0] = (data >> 24) & 0xFF;
    tx_data[1] = (data >> 16) & 0xFF;
    tx_data[2] = (data >> 8) & 0xFF;
    tx_data[3] = data & 0xFF;

    if(dev->write_func(tx_data, 4) == false)
    {
        return false;
    }

    dev->delay_func(80); // 80 ms delay for calibration (in data sheet 60 ms, but 80 ms is more stable)

    header = 0;
    header |= AD7190_COMM_W;
    header |= AD7190_COMM_ADDR(AD7190_REG_MODE);

    data = (header << 24);
    data |= AD7190_MODE_DEFAULT;
    data |= AD7190_MODE_SEL(AD7190_REG_FULLSCALE);

    tx_data[0] = (data >> 24) & 0xFF;
    tx_data[1] = (data >> 16) & 0xFF;
    tx_data[2] = (data >> 8) & 0xFF;
    tx_data[3] = data & 0xFF;
    if(dev->write_func(tx_data, 4) == false)
    {
        return false;
    }
    dev->delay_func(80); // 80 ms delay for calibration (in data sheet 60 ms, but 80 ms is more stable)

    return true;
}

bool _ad7190_get_data(ad7190_t* dev, ad7190_data_t *data)
{
    uint8_t header[1] = {AD7190_REG_DATA};
    uint8_t rx_data[4];

    if(dev->read_func(header, 1, rx_data, 4) == false)
    {
        return false;
    }

    data->status_register = rx_data[0];
    data->raw_data = rx_data[1] << 16 | rx_data[2] << 8 | rx_data[3];

    return true;
}

bool _ad7190_set_continuous_mode(ad7190_t* dev)
{
    uint8_t header = 0;
    header |= AD7190_COMM_W;
    header |= AD7190_COMM_ADDR(AD7190_REG_MODE);

    uint32_t data = (header << 24);
    data |= AD7190_MODE_DEFAULT;
    data |= AD7190_MODE_SEL(AD7190_MODE_CONTINUOUS);
    data |= AD7190_MODE_CLK_INT;
    data |= AD7190_MODE_DATA_STA;
    data |= AD7190_MODE_FILTER(AD7190_FILTER_WORD_8);

    uint8_t tx_data[4];
    tx_data[0] = (data >> 24) & 0xFF;
    tx_data[1] = (data >> 16) & 0xFF;
    tx_data[2] = (data >> 8) & 0xFF;
    tx_data[3] = data & 0xFF;

    if(dev->write_func(tx_data, 4) == false)
    {
        return false;
    }

    dev->delay_func(10); // 10 ms delay for setup (in data sheet 5 ms, but 10 ms is more stable)

    return true;
}

bool _ad7190_write_data(uint8_t* tx_data, size_t tx_len)
{
    if(_ad7190_spi_transmit(tx_data, tx_len, NULL, 0) == false)
    {
        return false;
    }

    return true;
}
bool _ad7190_read_data(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len)
{
    if(_ad7190_spi_transmit(tx_data, tx_len, rx_data, rx_len) == false)
    {
        return false;
    }

    return true;
}
