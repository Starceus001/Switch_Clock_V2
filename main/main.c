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

    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    // init display
    init_Display();     // init display also inits I2C so this needs to happen before init_ds3232

    // read RTC value into nvm_cfg
    xTaskCreatePinnedToCore(read_ds3232_task, "read_ds3232_task", 2048, NULL, 2, NULL, 0);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    // print cfg
    cfg_print();

    // start ms and display task (call only once on second core)
    // xTaskCreatePinnedToCore(updateElapsedTimeTask, "updateElapsedTimeTask", 4096*16, NULL, ESP_TASK_PRIO_MAX, NULL, 1);
    
    // init timer to interrupt each ms
    init_ms_outputs_intr();

    // create task to read cli
    xTaskCreatePinnedToCore(read_cli_constant, "read_cli_constant", 2048*16, NULL, 1, NULL, 0);

#if DISPLAY_ENABLE
    xTaskCreatePinnedToCore(Display_ssd1306, "Display_ssd1306", 4096*2, NULL, 4, NULL, 0);
#endif

    // main code (will repeat indefinitely)
    while (1) {
        // do not update clock time in cfg when in clock menu, this will remove your set time before saving within 10 seconds
        if (nvm_cfg.flags.clock_flag == 0) {
            // update clock time to cfg
            read_system_time_to_cfg();
        }
        
        // wait 10 second
        vTaskDelay(pdMS_TO_TICKS(10000));
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