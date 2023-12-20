// includes
#include "task.h"

// declare all ms timer functions in here
// start ms timer
void ms_timer_start(void *pvParameters) {
    // Access the cfg structure passed as a parameter
    cfg_t *Cfg = (cfg_t *)pvParameters;

    // this needs to be around the function code:
    if (xSemaphoreTake(cfgMutex, portMAX_DELAY)) {
        // Access and modify cfg from the second core

        // do stuff safely with the shared data struct so add Roan code here...
        
        // Release the mutex after accessing cfg
        xSemaphoreGive(cfgMutex);
    }




    // const esp_timer_create_args_t timer_args = {
    //     .callback = &timer_callback,
    //     .arg = NULL,
    //     .name = "periodic_timer"
    // };
 
    // ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timer));
 
    // // Start the timer with a period of 1000 microseconds (1ms)
    // ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000));
}
 