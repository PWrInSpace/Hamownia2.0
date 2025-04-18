// 06.04.2025 Bart3kTK

#include "ad7190_task.h"
#include "ad7190_wrapper.h"
#include "mcu_gpio_config.h"
#include "esp_log.h"

#define TAG "AD7190_TASK"
TaskHandle_t ad7190_task_handle = NULL;

void IRAM_ATTR my_gpio_isr(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (ad7190_task_handle != NULL) {
        xTaskNotifyFromISR(ad7190_task_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void setup_isr() {
    static const char* my_msg = "SPI_READY";
    if(_rdy_gpio_attach_isr(my_gpio_isr, (void*)my_msg))
    {
        ESP_LOGI(TAG, "GPIO ISR setup successful");
    }
    else
    {
        ESP_LOGE(TAG, "GPIO ISR setup failed");
        return;
    }

}

void ad7190_read_cont_data(void* pvParameters) {
    
    ad7190_delay_fnc(10);
    ad7190_data_t ad7190_data;
    while (1) {

        if(ad7190_get_data(&ad7190_data))
        {
            ESP_LOGD(TAG, "AD7190 data: %d", ad7190_data.raw_data);
        }
        else
        {
            ESP_LOGE(TAG, "AD7190 read data failed");
        }

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        //Ten task jest triggerowany przez ISR
        //W momencie gdy SPI jest gotowe do odczytu
        //Handler dziala bo przetestowalem na dev kit
        //TODO przeliczyc wartosci na jednostki
        //TODO zapisac dane do sd karty
    }
}

bool ad7190_prepare()
{
    if(ad7190_init())
    {
        ESP_LOGI(TAG, "AD7190 initialized successfully");
        return true;
    }
    else
    {
        ESP_LOGE(TAG, "AD7190 initialization failed");
        return false;
    }
    return false;
}
void ad7190_init_task()
{
    setup_isr();
    ad7190_set_continuous_mode();
    xTaskCreate(ad7190_read_cont_data, "ad7190_task", 4096, NULL, 10, &ad7190_task_handle);
}



