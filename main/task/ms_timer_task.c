// includes
#include "task.h"

#define MS_TIMER_TAG "MS_TIMER"

// declare all ms timer functions in here
// void init_ms_timer() {
//     // Create a task on the second core with ms_timer and pass the cfg struct
//     xTaskCreatePinnedToCore(
//         ms_timer,                   // Function to run on the second core
//         "ms_timer_second_core",     // Task name
//         10000,                      // Stack size (bytes)
//         NULL,                       // Task parameters (not needed in this example)
//         1,                          // Task priority
//         NULL,                       // Task handle (not needed in this example)
//         1                           // Core to run the task on (1 for the second core)
//     );
// }

// // ms timer
// void ms_timer(void *pvParameters) {
//     // Access the cfg structure passed as a parameter
//     cfg_t *Cfg = (cfg_t *)pvParameters;

//     // this needs to be around the function code:
//     if (xSemaphoreTake(cfgMutex, portMAX_DELAY)) {
//         // Access and modify cfg from the second core

//         // do stuff safely with the shared data struct so add Roan code here...
        
//         // Release the mutex after accessing cfg
//         xSemaphoreGive(cfgMutex);
//     }



// // OLD:
//     // const esp_timer_create_args_t timer_args = {
//     //     .callback = &timer_callback,
//     //     .arg = NULL,
//     //     .name = "periodic_timer"
//     // };
 
//     // ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timer));
 
//     // // Start the timer with a period of 1000 microseconds (1ms)
//     // ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000));
// }
 