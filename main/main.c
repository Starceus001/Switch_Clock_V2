// includes
#include "main.h"
#include "task.h"

void main(void)
{
    cfg_t cfg;
    nvm_cfg_t nvm_cfg;

    // run all init functions (to be declared inside task files in the task folder)
    // init ESP32 pinout
    init_gpio();

    // init display
    // init_display();
 
    // read RTC value into nvm_cfg
    // read_ds3232_task();

    // start ms timer on it's own core
    // ms_timer_start(); // On second core, in this function, check each ms all timers to set outputs
    // Create a task on the second core with ms_timer_start and pass the cfg struct
    xTaskCreatePinnedToCore(
        ms_timer_start,        // Function to run on the second core
        "ms_timer_second_core",// Task name
        10000,                 // Stack size (bytes)
        NULL,                  // Task parameters (not needed in this example)
        1,                     // Task priority
        NULL,                  // Task handle (not needed in this example)
        1                      // Core to run the task on (1 for the second core)
    );

    // main code (will repeat indefinitely)
    while (1) {
        // run all functionalities that will continue in sequence until the end
        // check inputs (buttons)
        // check_analog_inputs_task(); // on first core, read all buttons and set flags, then do stuff with the flags
    }
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