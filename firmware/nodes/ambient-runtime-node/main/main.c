#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("\n");
    printf("=====================================\n");
    printf(" Ambient Runtime Node\n");
    printf(" Ambient Physical AI\n");
    printf(" ESP32-P4 Bringup\n");
    printf("=====================================\n");

    while (1)
    {
        printf("Ambient Runtime Alive\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}