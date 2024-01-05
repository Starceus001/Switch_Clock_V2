// includes
#include "main.h"
#include "task.h"

#define MAIN_TAG "MAIN"

void app_main(void)
{
    // feedback
    startup_logs();

    cfg_t cfg;
    nvm_cfg_t nvm_cfg;

    // run all init functions (to be declared inside task files in the task folder)
    // init ESP32 pinout
    ESP_LOGI(MAIN_TAG, "Init GPIO");
    init_gpio();

    // init display
    ESP_LOGI(MAIN_TAG, "Init Display");
    // init_display();      // "TEST" To be implemented
 
    // read RTC value into nvm_cfg
    ESP_LOGI(MAIN_TAG, "Init RTC");
    // read_ds3232_task();      // "TEST" To be implemented

    // init ms timer that checks scheduled switchmoments and sets outputs
    ESP_LOGI(MAIN_TAG, "Init ms timer");
    // init_ms_timer(); // inside this function, start the timer when the rtc has given it's 1 sec flag (to be in sync with the second)

    // main code (will repeat indefinitely)
    while (1) {
        // run all functionalities that will continue in sequence until the end
        // do stuff with buttons flags on every run?

        // "TEST"
        ESP_LOGW("TEST", "looping through main for infinity and beyond!");  // "TEST"
        
        vTaskDelay(pdMS_TO_TICKS(1000));  // "TEST" Sleep for 1000 milliseconds (1 second)
    }
}

void startup_logs() {
    // feedback
    ESP_LOGI(MAIN_TAG, "Starting up...");

    // feedback project creators
    ESP_LOGI(MAIN_TAG, "Project created by:");
    ESP_LOGI(MAIN_TAG, "------------------------------------------------------");
    ESP_LOGI(MAIN_TAG, "| Guido Annema - guido.annema@student.nhlstenden.com |");
    ESP_LOGI(MAIN_TAG, "| Roan Post - roan.post@student.nhlstenden.com       |");
    ESP_LOGI(MAIN_TAG, "------------------------------------------------------");

    // feedback project information
    ESP_LOGI(MAIN_TAG, "Project information:");
    ESP_LOGI(MAIN_TAG, "------------------------------------------------------");
    ESP_LOGI(MAIN_TAG, "| Project: Schakelklok                               |");
    ESP_LOGI(MAIN_TAG, "| Education: HBO Elektrotechniek                     |");
    ESP_LOGI(MAIN_TAG, "| Institute: NHL Stenden Leeuwarden                  |");
    ESP_LOGI(MAIN_TAG, "| Teacher: Rieno Moedt                               |");
    ESP_LOGI(MAIN_TAG, "| Date: 04-01-2024                                   |");
    ESP_LOGI(MAIN_TAG, "------------------------------------------------------");
}



// NOTES:
// this needs to be around the function code when the code accesses shared data by multiple cores
// above the file:
// SemaphoreHandle_t cfgMutex;  // Mutex to synchronize access to cfg

// inside the function:
// if (xSemaphoreTake(cfgMutex, portMAX_DELAY)) {
//     // Access and modify cfg from the second core
//     // do stuff safely with the shared data struct.

//     // Release the mutex after accessing cfg
//     xSemaphoreGive(cfgMutex);
// }