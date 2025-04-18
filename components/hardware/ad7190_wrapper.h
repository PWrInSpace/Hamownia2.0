// 06.04.2025 Bart3kTK

#ifndef AD7190_WRAPPER_H
#define AD7190_WRAPPER_H

#include <stdbool.h>
#include "ad7190.h"

void ad7190_delay_fnc(uint32_t millis);

bool ad7190_init(void);

bool ad7190_check_id(void);

bool ad7190_calibrate(void);

bool ad7190_reset(void);

bool ad7190_setup(void);

bool ad7190_set_continuous_mode(void);

bool ad7190_get_data(ad7190_data_t *data);

bool ad7190_write_data(uint8_t* rx_data, size_t rx_len);

bool ad7190_read_data(const uint8_t* tx_data, size_t tx_len, uint8_t* rx_data, size_t rx_len);

#endif // AD7190_WRAPPER_H