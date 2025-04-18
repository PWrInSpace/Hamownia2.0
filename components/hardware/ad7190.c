// 06.04.2025 Bart3kTK

#include "ad7190.h"
#define TAG "AD7190"
#include "esp_log.h"

bool _ad7190_init(ad7190_t* dev)
{
    ESP_LOGI(TAG, "start init");

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

    ESP_LOGI(TAG, "init done");

    return true;
}

bool _ad7190_setup(ad7190_t* dev)
{
    ESP_LOGI(TAG, "start setup");


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
        ESP_LOGE(TAG, "write_func failed");
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

    ESP_LOGD(TAG, "setup done");
    return true;
}

bool _ad7190_reset(ad7190_t* dev)
{

    ESP_LOGD(TAG, "start reset");

    uint64_t data = AD7190_DEV_RESET;
    uint8_t tx_data[5];
    tx_data[0] = (data >> 32) & 0xFF;
    tx_data[1] = (data >> 24) & 0xFF;
    tx_data[2] = (data >> 16) & 0xFF;
    tx_data[3] = (data >> 8) & 0xFF;
    tx_data[4] = data & 0xFF;
    if(dev->write_func(tx_data, 5) == false)
    {
        ESP_LOGE(TAG, "write_func failed");
        return false;
    }
    dev->delay_func(10); // 10 ms delay for reset (in data sheet 0.5 ms, but 10 ms is more stable)


    ESP_LOGD(TAG, "reset done");

    return true;
}
bool _ad7190_check_id(ad7190_t* dev)
{

    ESP_LOGD(TAG, "check id");

    uint8_t header = 0;
    header |= AD7190_COMM_R;
    header |= AD7190_COMM_ADDR(AD7190_REG_ID);

    uint8_t tx_data[1];
    tx_data[0] = header;

    uint8_t rx_data[1];

    if(dev->read_func(tx_data, 1, rx_data, 1) == false)
    {
        ESP_LOGE(TAG, "check id read_func failed");
        return false;
    }
    if(rx_data[0] != AD7190_DEV_ID)
    {
        ESP_LOGE(TAG, "check id failed");
        return false;
    }

    ESP_LOGD(TAG, "id check done");

    return true;

}

bool _ad7190_calibrate(ad7190_t* dev)
{

    ESP_LOGD(TAG, "start calibration");

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
        ESP_LOGE(TAG, "write_func failed in calibration");
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
        ESP_LOGE(TAG, "write_func failed in calibration 2");
        return false;
    }
    dev->delay_func(80); // 80 ms delay for calibration (in data sheet 60 ms, but 80 ms is more stable)

    ESP_LOGD(TAG, "calibration done");

    return true;
}

bool _ad7190_get_data(ad7190_t* dev, ad7190_data_t *data)
{
    ESP_LOGD(TAG, "start read data in continuous mode");

    /* Read data in continuous mode */
    uint8_t header[1] = {AD7190_REG_DATA};
    uint8_t rx_data[4];

    if(dev->read_func(header, 1, rx_data, 4) == false)
    {
        ESP_LOGE(TAG, "read_func failed when reading data");
        return false;
    }

    data->status_register = rx_data[0];
    data->raw_data = rx_data[1] << 16 | rx_data[2] << 8 | rx_data[3];

    ESP_LOGD(TAG, "read data done in continuous mode");

    return true;
}

bool _ad7190_set_continuous_mode(ad7190_t* dev)
{
    ESP_LOGD(TAG, "start set continuous mode");

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
        ESP_LOGE(TAG, "write_func failed in set continuous mode");
        return false;
    }

    dev->delay_func(10); // 10 ms delay for setup (in data sheet 5 ms, but 10 ms is more stable)

    ESP_LOGD(TAG, "set continuous mode done");
    return true;
}
