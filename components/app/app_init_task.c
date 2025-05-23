///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 25.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "app_init_task.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "mcu_gpio_config.h"
#include "mcu_spi_config.h"
#include "mcu_adc_config.h"
#include "timers_config.h"
#include "TANWA_config.h"

#include "ad7190_task.h"

#include "sd_task.h"


#define TAG "APP_INIT_TASK"

#define APP_INIT_TASK_STACK_SIZE 4096
#define APP_INIT_TASK_PRIORITY 1
#define APP_INIT_TASK_CORE 0

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

void run_app_init(void) {
  xTaskCreatePinnedToCore(app_init_task, "app_init_task", APP_INIT_TASK_STACK_SIZE, NULL, APP_INIT_TASK_PRIORITY, NULL, APP_INIT_TASK_CORE);
}

void app_init_task(void* pvParameters) {
  ESP_LOGI(TAG, "### App initialization task started ###");
  esp_err_t ret = ESP_OK;

  ESP_LOGI(TAG, "Initializing SPI...");

  ret |= mcu_spi_init();
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialize SPI");
  } else {
      ESP_LOGI(TAG, "SPI initialized");
  }

  ESP_LOGI(TAG, "Initializing SD Card...");

  if (!init_sd_card()) {
    ESP_LOGE(TAG, "SD Card initialization failed");
  } else {
    ESP_LOGI(TAG, "### SD Card initialization success ###");
  }

  ESP_LOGI(TAG, "Initializing MCU configuration...");

  ret |= TANWA_mcu_config_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "MCU configuration failed");
  } else {
    ESP_LOGI(TAG, "### MCU configuration success ###");
  }

  ESP_LOGI(TAG, "Initializing hardware...");
  
  ret |= TANWA_hardware_init(); //problemy
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Hardware initialization failed");
  } else {
    ESP_LOGI(TAG, "### Hardware initialization success ###");
  }

  ESP_LOGI(TAG, "Initializing utility...");

  ret |= TANWA_utility_init(); //problemy
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Utility initialization failed");
  } else {
    ESP_LOGI(TAG, "### Utility initialization success ###");
  }

  // ESP_LOGI(TAG, "Initializing state machine...");

  // if (!initialize_state_machine()) {
  //   ESP_LOGE(TAG, "State machine initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "### State machine initialization success ###");
  // }

  // ESP_LOGI(TAG, "Initializing ESP-NOW...");

  // ret |= TANWA_esp_now_init();
  // if (ret != ESP_OK) {
  //   ESP_LOGE(TAG, "ESP-NOW initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "### ESP-NOW initialization success ###");
  // }

  // ESP_LOGI(TAG, "Initializing shared memory...");

  // if (!tanwa_data_init()) {
  //   ESP_LOGE(TAG, "Shared memory initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "### Shared memory initialization success ###");
  // }

  // if (!initialize_timers()) {
  //   ESP_LOGE(TAG, "Timers initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "### Timers initialization success ###");
  // }

  // SD CARD TIMER 
  if (!sys_timer_start(TIMER_SD_DATA, 50, TIMER_TYPE_PERIODIC)) {
    ESP_LOGE(TAG, "SD CARD | Timer start failed");
  } else {
    ESP_LOGI(TAG, "SD CARD | Timer started");
  }

  // ESP_LOGI(TAG, "Initializing LoRa...");

  // if (!initialize_lora(LORA_TASK_FREQUENCY_KHZ, LORA_TASK_TRANSMIT_MS)) {
  //   ESP_LOGE(TAG, "LoRa initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "### LoRa initialization success ###");
  // }

  if(ad7190_prepare() == false)
  {
      ESP_LOGE(TAG, "AD7190 initialization failed");
  }
  else
  {
      ESP_LOGI(TAG, "AD7190 initialized");
  }
  ESP_LOGI(TAG, "### App initialization finished ###");

  // ESP_LOGI(TAG, "Starting console...");

  // ret |= console_config_init();
  // if (ret != ESP_OK) {
  //   ESP_LOGE(TAG, "Console initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "Console initialized");
  // }

  // state_machine_change_state(IDLE);

  // run_can_task();
  // vTaskDelay(pdMS_TO_TICKS(10));
  // run_measure_task();
  // vTaskDelay(pdMS_TO_TICKS(100));
  // run_esp_now_task();

  // ESP_LOGI(TAG, "Initializating abort button...");
  // ret |= abort_button_init();
  // if (ret != ABORT_BUTTON_OK) {
  //   ESP_LOGE(TAG, "Abort button initialization failed");
  // } else {
  //   ESP_LOGI(TAG, "Abort button initialized");
  // }
  ad7190_init_task();

  vTaskDelete(NULL);
}