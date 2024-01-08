// includes
#include "task.h"

#define MS_TIMER_TAG "MS_TIMER"

// declare all ms timer functions in here
// function will run every ms to steer display and ms timer
void updateElapsedTimeTask(void *pvParameters) {
    // define function variables
    uint64_t elapsedTimeMillis = 0;             // global variable to store elapsed time
    struct timeval lastUpdateTime;
    gettimeofday(&lastUpdateTime, NULL);
    static int ms500Executed = 0;               // one time execute variable
    static uint64_t elapsedTimeCounter = 100;   // declare a counter to keep track of elapsed time

    // infinite while loop in task
    while (1) {
        // 
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);       //Null is voor tijdzone instellen

        // calculate elapsed time in milliseconds
        elapsedTimeMillis = (uint64_t)(currentTime.tv_sec - lastUpdateTime.tv_sec) * 1000 +
                            (uint64_t)(currentTime.tv_usec - lastUpdateTime.tv_usec) / 1000;




        // LOOP TASK
        // check if 100 milliseconds have passed and update display
        if (elapsedTimeMillis >= elapsedTimeCounter) {
            // update display every 100 milliseconds
            xTaskCreatePinnedToCore(Display_ssd1306, "Display_ssd1306", 4096*2, NULL, 3 ,NULL, 0);
            // reset the counter only if it's greater than or equal to 100
            elapsedTimeCounter += 100;
        }

        // reset vars at the end of each second
        if (elapsedTimeMillis >= 1000) {
            // Reset the counter
            elapsedTimeCounter = 0;
            lastUpdateTime = currentTime;
            elapsedTimeMillis = 0;
        }
    }
}