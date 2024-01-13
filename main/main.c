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

    vTaskDelay(50 / portTICK_PERIOD_MS);
    
    // init display
    init_Display();     // init display also inits I2C so this needs to happen before init_ds3232

    // read RTC value into nvm_cfg
    xTaskCreate(read_ds3232_task, "ds3232_task", 2048, NULL, 2, NULL);

    // print cfg
    cfg_print();

    // start ms and display task (call only once on second core)
    xTaskCreatePinnedToCore(updateElapsedTimeTask, "updateElapsedTimeTask", 4096*16, NULL, ESP_TASK_PRIO_MAX ,NULL, 1);

    // create task to read cli
    xTaskCreate(read_cli_constant, "read_cli_constant", 2048*16, NULL, 1, NULL);

    // main code (will repeat indefinitely)
    while (1) {
        // wait 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// print start logs
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