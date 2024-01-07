// includes
#include "main.h"
#include "task.h"

#define MAIN_TAG "MAIN"

// Define the structs
cfg_t cfg;
nvm_cfg_t nvm_cfg;

void app_main(void)
{
    // feedback
    startup_logs();

    // run all init functions (declared inside task files in the task folder)
    // init cfg
    init_cfg();

    // init ESP32 pinout
    init_gpio();

    // read RTC value into nvm_cfg
    ESP_LOGI(MAIN_TAG, "Reading RTC");    // move into init function into task folder
    // read_ds3232_task();      // "TEST" To be implemented

    // init display
    ESP_LOGI(MAIN_TAG, "Init Display");    // move into init function into task folder
    // init_display();      // "TEST" To be implemented

    // init ms timer that checks scheduled switchmoments and sets outputs
    ESP_LOGI(MAIN_TAG, "Init ms timer");    // move into init function into task folder
    // init_ms_timer(); // inside this function, start the timer when the rtc has given it's 1 sec flag (to be in sync with the second)

    // print cfg
    cfg_print();

    // main code (will repeat indefinitely)
    while (1) {
        // run all functionalities that will continue in sequence until the end
        // do stuff with buttons flags on every run?

        ESP_LOGW("TEST", "looping through main for infinity and beyond!");
        
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
    ESP_LOGI(MAIN_TAG, "| Date: 07-01-2024                                   |");
    ESP_LOGI(MAIN_TAG, "------------------------------------------------------");
}