// 06.04.2025 Bart3kTK

#include "ad7190_wrapper.h"
#include "ad7190.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include "mcu_spi_config.h"
#include "esp_log.h"

#define TAG "AD7190_WRAPPER"


void ad7190_delay_fnc(uint32_t millis) {
    vTaskDelay(pdMS_TO_TICKS(millis));
}

bool _ad7190_write_data(uint8_t* tx_data, size_t tx_len)
{
    ESP_LOGD(TAG, "start transmit data to SPI");

    if(_ad7190_spi_transmit(tx_data, tx_len, NULL, 0) == false)
    {
        return false;
    }

    ESP_LOGD(TAG, "transmit data to SPI done");

    return true;
}
bool _ad7190_read_data(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len)
{
    ESP_LOGI(TAG, "start read data from SPI");

    if(_ad7190_spi_transmit(tx_data, tx_len, rx_data, rx_len) == false)
    {
        return false;
    }

    ESP_LOGD(TAG, "read data from SPI done");

    return true;
}

static struct {
    ad7190_t dev;
} gb;


bool ad7190_init(void) {
    gb.dev.dev_addr = AD7190_DEV_ID;
    gb.dev.read_func = _ad7190_read_data;
    gb.dev.write_func = _ad7190_write_data;
    gb.dev.delay_func = ad7190_delay_fnc;
    return _ad7190_init(&gb.dev);
}

bool ad7190_check_id(void) {
    return _ad7190_check_id(&gb.dev);
}

bool ad7190_calibrate(void) {
    return _ad7190_calibrate(&gb.dev);
}

bool ad7190_reset(void) {
    return _ad7190_reset(&gb.dev);
}

bool ad7190_setup(void) {
    return _ad7190_setup(&gb.dev);
}

bool ad7190_set_continuous_mode(void) {
    return _ad7190_set_continuous_mode(&gb.dev);
}

bool ad7190_get_data(ad7190_data_t *data) {
    return _ad7190_get_data(&gb.dev, data);
}

bool ad7190_write_data(uint8_t* rx_data, size_t rx_len) {
    return _ad7190_write_data(rx_data, rx_len);
}

bool ad7190_read_data(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len) {
    return _ad7190_read_data(tx_data, tx_len, rx_data, rx_len);
}

//TODO: tu bedzie jeszcze funckja do przeliczania raw_data na nacisk