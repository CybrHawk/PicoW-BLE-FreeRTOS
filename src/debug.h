#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//Prototypes for the Optionnal debug commands

void vLed_task1(void *pvParameters);
void vLed_task2(void *pvParameters);
void vLed_task3(void *pvParameters);