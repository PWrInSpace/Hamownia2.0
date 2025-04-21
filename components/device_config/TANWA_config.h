///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_CONFIG_H_
#define PWRINSPACE_TANWA_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#include "pca9574.h"
#include "sdcard.h"
#include "igniter_driver.h"
#include "buzzer_driver.h"
#include "mcp23018.h"

#include "pressure_driver.h"
#include "solenoid_driver.h"

// TANWA hardware
typedef struct {
    mcp23018_struct_t mcp23018;
    ads1115_struct_t ads1115;
    pca9574_struct_t pca9574;
    igniter_struct_t igniter[2];
    buzzer_struct_t buzzer;
} TANWA_hardware_t;

// TANWA utilities 
typedef struct {
    pressure_driver_struct_t pressure_driver;
    solenoid_driver_struct_t solenoid_driver;
} TANWA_utility_t;

// LoRa communication

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

esp_err_t TANWA_mcu_config_init();

esp_err_t TANWA_hardware_init();

esp_err_t TANWA_utility_init();

esp_err_t TANWA_esp_now_init();

// esp_err_t TANWA_get_hardware(TANWA_hardware_t** hardware);

// esp_err_t TANWA_get_utility(TANWA_utility_t** utility);

// esp_err_t TANWA_get_vbat(float* vbat);

#endif /* PWRINSPACE_TANWA_CONFIG_H_*/