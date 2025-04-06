// 06.04.2025 Bart3kTK

#ifndef AD7190_TASK_H
#define AD7190_TASK_H

#include <stdbool.h>

void ad7190_read_cont_data(void* pvParameters);
bool ad7190_prepare();
void ad7190_init_task();
#endif // AD7190_TASK_H