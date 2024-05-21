#include <stdio.h>
#include <stdlib.h> // malloc() free(), mandatory
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//#include "debug.h" //Used for debugging function, such as LEDs and additionnal buttons

//LCD
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include <stdio.h>
#include "hardware/watchdog.h"
//#include "pico/stdlib.h"



static QueueHandle_t xQueue = NULL;

SemaphoreHandle_t mutex;


void vUSB_task(void *pvParameters) {
    uint uIReceivedValue = 0;
    while (true) {
        xQueueReceive(xQueue, &uIReceivedValue, portMAX_DELAY); // Receive from the queue with a block time of 0
        if(uIReceivedValue == 1) {
          printf("HB LED ON\n");
        }
        else {
          printf("HB LED OFF\n");
        }
    }
}

void vLedHB_task() 
{
    uint uIValueToSend = 0;
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        uIValueToSend = 1;
        xQueueSend(xQueue, &uIValueToSend, 0U); // Send to the queue without delay, 0U because queue should always be empty
        vTaskDelay(pdMS_TO_TICKS(1000));
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        uIValueToSend = 0;
        xQueueSend(xQueue, &uIValueToSend, 0U);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vLCD_task(void *pvParameters) {
    printf("LCD_test Demo\r\n");
    uint8_t counter = 0;
    Button myButton = {00, 270, 240, 320};

    System_Init();
    LCD_SCAN_DIR  lcd_scan_dir = SCAN_DIR_DFT;
    LCD_Init(lcd_scan_dir,800);
    TP_Init(lcd_scan_dir);

    //GUI_Show(); //Test if the GUI is working via the example code
    //Driver_Delay_ms(1500);

    TP_Adjust(); // Uncomment this line to calibrate the touchscreen
    drawButton(myButton); // Draw the button for the first time

    while(true) {
        // Check if the touchscreen is pressed

        if (isButtonPressed(myButton, lcd_scan_dir)) {
            button_pressed();
            drawButton(myButton);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    vTaskDelete(NULL); // Delete this task (NULL = this task), normally never reached
}

void vBLE_client_task() {
    //Envoyer les infos du capteur a l'ecran via Queue

    while(true){
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL); // Delete this task (NULL = this task), normally never reached
}


void main(){
    stdio_init_all();

    if (cyw43_arch_init()) { //Init for the WiFi BLE Chip
        printf("Wi-Fi init failed");
    }

    mutex = xSemaphoreCreateMutex(); // Create the mutex
    xQueue = xQueueCreate(1, sizeof(uint)); // Create the queue

    #if FORCECORES
    // Define the task handles
    TaskHandle_t handleA;
    TaskHandle_t handleB;
    #endif

    xTaskCreate(vUSB_task, "USB_Task", 256, NULL, 4, NULL);

    xTaskCreate(vLedHB_task, "HB_Task", 256, NULL, 5, NULL);

    xTaskCreate(vLCD_task, "LCD_Task", 4096, NULL, 6, NULL);

    xTaskCreate(vBLE_client_task, "BLE_Task", 2048, NULL, 4, NULL);

    #if DEBUG
    xTaskCreate(vLed_task1, "LED_Task1", 256, NULL, 3, NULL);
    xTaskCreate(vLed_task2, "LED_Task2", 256, NULL, 3, NULL);
    xTaskCreate(vLed_task3, "LED_Task3", 256, NULL, 3, NULL);
    #endif

    #if FORCECORES
    // Force Cores Affinity
    vTaskCoreAffinitySet(handleA, (1 << 0)); // Core 0
    vTaskCoreAffinitySet(handleB, (1 << 1)); // Core 1
    #endif

    // Start the scheduler
    vTaskStartScheduler();
}