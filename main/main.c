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
    // init_display();      // "TEST" To be implemented
 
    // read RTC value into nvm_cfg
    // read_ds3232_task();      // "TEST" To be implemented

    // init ms timer that checks scheduled switchmoments and sets outputs
    // init_ms_timer();

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