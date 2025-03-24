#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


#include "app_init_task.h"

#include "sd_task.h"

void app_main(void)
{
    run_app_init();
}
