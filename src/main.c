#include <stdio.h>
//#include "pico/stdlib.h" //Cause problemes de debugage multitreahd ?
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "debug.h"

//OLED
#include <stdlib.h> // malloc() free(), mandatory
#include "OLED_1in3_c.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"

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

void vOLED_task(void *pvParameters)
{
    
    printf("OLED_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        while(1){
            printf("END\r\n");
        }
    }
    
    /* Init */
    OLED_1in3_C_Init();
    OLED_1in3_C_Clear();

    
    UBYTE *BlackImage;
    UWORD Imagesize = ((OLED_1in3_C_WIDTH%8==0)? (OLED_1in3_C_WIDTH/8): (OLED_1in3_C_WIDTH/8+1)) * OLED_1in3_C_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        while(1){
            printf("Failed to apply for black memory...\r\n");
        }
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);	
    
    
    
    printf("Drawing\r\n");
vTaskDelay(pdMS_TO_TICKS(4000));
    Paint_SelectImage(BlackImage);
    Paint_Clear(BLACK);
    

    printf("Begin Drawing:page 1\r\n");
    Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    BlackImage[0] = 0xf0;

    OLED_1in3_C_Display(BlackImage);

    vTaskDelay(pdMS_TO_TICKS(1000));

    Paint_Clear(BLACK);
    Paint_DrawString_EN(10, 0, "Pico-OLED", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 15, "Built on", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 30, __TIME__" ", &Font16, WHITE, BLACK);
    OLED_1in3_C_Display(BlackImage);

    vTaskDelay(pdMS_TO_TICKS(100));
    printf("Done Drawing:page 1\r\n");

    vTaskDelete(NULL); // Delete this task (NULL = this task), then while loop never reached

    while (true) { // This loop is never reached if TaskDelete worked
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("OLED IDLE\r\n");
    }
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

    xTaskCreate(vOLED_task, "OLED_Task", 2048, NULL, 6, NULL); //Forcing on a handle doest work; why ? 

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