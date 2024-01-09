// includes
#include "task.h"

#define MS_TIMER_TAG "MS_TIMER"

// declare all ms timer functions in here
// function will run every ms to steer display and ms timer
void updateElapsedTimeTask(void *pvParameters) {
    // define function variables
    uint64_t elapsedTimeMillis = 0;             // global variable to store elapsed time
    struct timeval lastUpdateTime;              // declare a variable to store the last update time
    gettimeofday(&lastUpdateTime, NULL);        // get the current time and store it in the lastUpdateTime variable when first running this (only once)
    static uint64_t elapsedTimeCounter = 100;   // declare a counter to keep track of elapsed time

    // infinite while loop in task
    while (1) {
        // "TEST" what does this do??
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);

        // calculate elapsed time in milliseconds
        elapsedTimeMillis = (uint64_t)(currentTime.tv_sec - lastUpdateTime.tv_sec) * 1000 +
                            (uint64_t)(currentTime.tv_usec - lastUpdateTime.tv_usec) / 1000;

        // check each ms all timers against system time
        check_timers_time_to_system_time(elapsedTimeMillis);

        // LOOP TASK
        // check if 100 milliseconds have passed to update display
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

            // set system time to cfg
            read_system_time_to_cfg();
        }
    }
}

// check if timer moment has arrived
void check_timers_time_to_system_time(uint16_t counting_ms) {
    // loop over each timer
    for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
        // check if timer moment is here
        if (cfg.timers[i].set_day == nvm_cfg.rtc.day &&
            cfg.timers[i].set_hour == nvm_cfg.rtc.hour &&
            cfg.timers[i].set_min == nvm_cfg.rtc.min &&
            cfg.timers[i].set_sec == nvm_cfg.rtc.sec) {
            if (cfg.timers[i].set_ms == counting_ms) {
                if ((cfg.timers[i].repeat_timer == 1 || cfg.timers[i].repeat_timer == 2) && (cfg.timers[i].setting_timer_output_useonce == false)) {
                    // set timer output regularly only once, next cycle is repeat
                    set_timer_output(i);

                    // call function to set repeat timer
                    timer_start_periodic(cfg.timers[i].interval_in_ms, i);

                    // set flag to true
                    cfg.timers[i].setting_timer_output_useonce = true;
                } else {
                    // set output
                    set_timer_output(i);
                }
            }
        }
    }
}

// set timer output when timer moment has arrived
void set_timer_output(uint8_t timer_number) {
    // check if timer is active
    if (cfg.timers[timer_number].timer_active == true) {
        gpio_set_level(output_pins[timer_number], cfg.timers[timer_number].set_value);
    }
}

// timer callback function
void timer_callback(void* arg) {
    // Cast the argument to the appropriate type
    int timer_index = (int)arg;

    // check if the repeat_timer is set
    if (cfg.timers[timer_index].repeat_timer == 0) {
        // stop the timer if the flag is set
        esp_timer_stop(periodic_timers[timer_index]);

        // make sure the flag is set to false so we can do our first run again
        cfg.timers[timer_index].setting_timer_output_useonce = false;

        // feedback
        ESP_LOGI(MS_TIMER_TAG, "Timer [%d] stopped due to repeat_timer", timer_index);
        
        // exit the callback
        return;
    }
    
    // toggle the specified output
    gpio_set_level(output_pins[timer_index], !gpio_get_level(output_pins[timer_index]));
}

// function to start a periodic timer
void timer_start_periodic(uint32_t mseconds, int timer_index) {  // msecond is full interval in ms
    // create timer arguments
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .arg = (void*)timer_index,
        .name = "periodic_timer"
    };

    // set the output pin associated with the timer
    // output_pin = output_pins[timer_index];       // "TEST" why do we have this? we are not using it??

    // create repeat timer
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timers[timer_index]));

    // start the repeat timer
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timers[timer_index], mseconds));
}