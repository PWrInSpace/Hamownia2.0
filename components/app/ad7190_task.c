// 06.04.2025 Bart3kTK

#include "ad7190_task.h"
#include "ad7190_wrapper.h"
#include "mcu_gpio_config.h"
#include "esp_log.h"

#define TAG "AD7190_TASK"

void ad7190_read_cont_data(void* pvParameters) {

    ad7190_data_t ad7190_data;

    while (1) {
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        
        if(ad7190_get_data(&ad7190_data))
        {
            ESP_LOGI(TAG, "AD7190 data: %d", ad7190_data.raw_data);
        }
        else
        {
            ESP_LOGE(TAG, "AD7190 read data failed");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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
    ad7190_set_continuous_mode();
    xTaskCreate(ad7190_read_cont_data, "ad7190_task", 4096, NULL, 10, &ad7190_task_handle);
}



