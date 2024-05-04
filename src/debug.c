#include "debug.h"

void vLed_task1(void *pvParameters) 
{
    const uint LED_PIN = 26;
    const uint BUTTON_PIN = 15;
    gpio_init(LED_PIN);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    while (true) {

        if(gpio_get(BUTTON_PIN)) {
            gpio_put(LED_PIN, 1);
        } else {
            gpio_put(LED_PIN, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vLed_task2(void *pvParameters) 
{
    const uint LED_PIN = 27;
    const uint BUTTON_PIN = 14;
    gpio_init(LED_PIN);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    while (true) {

        if(gpio_get(BUTTON_PIN)) {
            gpio_put(LED_PIN, 1);
        } else {
            gpio_put(LED_PIN, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vLed_task3(void *pvParameters) 
{
    const uint LED_PIN = 28;
    const uint BUTTON_PIN = 13;
    gpio_init(LED_PIN);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    while (true) {

        if(gpio_get(BUTTON_PIN)) {
            gpio_put(LED_PIN, 1);
        } else {
            gpio_put(LED_PIN, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}