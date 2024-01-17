// includes
#include "task.h"

#define MS_TIMER_TAG "MS_TIMER"

// outputs
const gpio_num_t output_pins[4] = {OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4};

// periodic timers (NOT actual cfg timers!)
esp_timer_handle_t periodic_timers[MAX_TIMER_COUNT];

esp_timer_handle_t periodic_timers_all;

// declare all ms timer functions in here
// function will run every ms to steer display and ms timer
void updateElapsedTimeTask(void *pvParameters) {
    // define function variables
    // struct timeval currentTime;
    TickType_t lastUpdateTime = xTaskGetTickCount(); // get current time in ticks and save (at start of task only)

    // // get current time in ms and save, will add 1 every ms run (in the while loop)
    // gettimeofday(&currentTime, NULL); 
    // uint32_t current_time_ms = (uint64_t)currentTime.tv_sec * 1000 +
    //                     (uint64_t)currentTime.tv_usec / 1000;

    // calculate ms time from start of week (rtc time)
    nvm_cfg.flags.current_time_ms = (uint64_t)nvm_cfg.rtc.day * 86400000 +       // "TEST" current_time_ms needs to reset to 0 when week in counting has finished and a new week (00:00:00:00:000) has begun
                                    (uint64_t)nvm_cfg.rtc.hour * 3600000 +
                                    (uint64_t)nvm_cfg.rtc.min * 60000 +
                                    (uint64_t)nvm_cfg.rtc.sec * 1000;

    // ESP_LOGE("TEST", "init current_time_ms: %llu", nvm_cfg.flags.current_time_ms);

    // infinite while loop in task
    while (1) {
        // set schakelmoment outputs based on previously calculated values
        // offering clean code for faster switching of outputs
        if (nvm_cfg.flags.set_out[0] == true) {
            gpio_set_level(output_pins[0], cfg.timers[0].set_value);
            // ESP_LOGE("TEST", "setting output 0 to %d", cfg.timers[0].set_value);
        }
        if (nvm_cfg.flags.set_out[1] == true) {
            gpio_set_level(output_pins[1], cfg.timers[1].set_value);
            // ESP_LOGE("TEST", "setting output 1 to %d", cfg.timers[1].set_value);
        }
        if (nvm_cfg.flags.set_out[2] == true) {
            gpio_set_level(output_pins[2], cfg.timers[2].set_value);
            // ESP_LOGE("TEST", "setting output 2 to %d", cfg.timers[2].set_value);
        }
        if (nvm_cfg.flags.set_out[3] == true) {
            gpio_set_level(output_pins[3], cfg.timers[3].set_value);
            // ESP_LOGE("TEST", "setting output 3 to %d", cfg.timers[3].set_value);
        }

        // toggle outputs if repeat moment arrived
        if (nvm_cfg.flags.rep_out[0] == true) {
            gpio_set_level(output_pins[0], !nvm_cfg.flags.out_read_value[0]);
            // ESP_LOGE("TEST", "toggling output 0 - nvm_cfg.flags.out_read_value[0]: %d", nvm_cfg.flags.out_read_value[0]);
        }
        if (nvm_cfg.flags.rep_out[1] == true) {
            gpio_set_level(output_pins[1], !nvm_cfg.flags.out_read_value[1]);
            // ESP_LOGE("TEST", "toggling output 1 - nvm_cfg.flags.out_read_value[1]: %d", nvm_cfg.flags.out_read_value[1]);
        }
        if (nvm_cfg.flags.rep_out[2] == true) {
            gpio_set_level(output_pins[2], !nvm_cfg.flags.out_read_value[2]);
            // ESP_LOGE("TEST", "toggling output 2 - nvm_cfg.flags.out_read_value[2]: %d", nvm_cfg.flags.out_read_value[2]);
        }
        if (nvm_cfg.flags.rep_out[3] == true) {
            gpio_set_level(output_pins[3], !nvm_cfg.flags.out_read_value[3]);
            // ESP_LOGE("TEST", "toggling output 3 - nvm_cfg.flags.out_read_value[3]: %d", nvm_cfg.flags.out_read_value[3]);
        }

        // calculate values for next iteration
        for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
            if (cfg.timers[i].timer_active == 1) {
                // has set time been reached?
                if ((cfg.timers[i].set_time_in_ms+1 == nvm_cfg.flags.current_time_ms+1) && (nvm_cfg.flags.first_run_done[i] == false)) {
                    // ESP_LOGE("TEST", "nvm_cfg.flags.set_out[%d] = true", i);
                    nvm_cfg.flags.set_out[i] = true;
                    // ESP_LOGE("TEST", "timer [%d] repeat_timer = %d", i, cfg.timers[i].repeat_timer);
                    if (cfg.timers[i].repeat_timer == 2) {
                        // ESP_LOGE("TEST", "setting nvm_cfg.flags.first_run_done[%d] = true", i);
                        nvm_cfg.flags.first_run_done[i] = true;
                    }
                }
                // no set time, check repeat time if set has been done at least once
                else {
                    nvm_cfg.flags.set_out[i] = false;
                    // did we stop timer from repeating?
                    if (cfg.timers[i].repeat_timer == 0) {
                        // ESP_LOGE("TEST", "repeat_timer[%d] has been turned off, reset flag and go back to checking set times, no repeats from here on out!", i);
                        nvm_cfg.flags.first_run_done[i] = false;
                    }
                    // no set time, are we sitll repeating and has set been done once?
                    if (nvm_cfg.flags.first_run_done[i] == true && cfg.timers[i].repeat_timer == 2) {
                        // check if timer is repeat (calculate time passed since set moment, divide it by repeat_interval_ms using modulus to get the remaining value. If it is not equal to 0, next iteration is not a repeat moment)
                        if ((((nvm_cfg.flags.current_time_ms+1) - (cfg.timers[i].set_time_in_ms+1)) % cfg.timers[i].interval_in_ms) == 0) {
                            // ESP_LOGE("TEST", "Next iteration, timer [%d] should repeat so nvm_cfg.flags.rep_out[%d] = true", i, i);
                            nvm_cfg.flags.rep_out[i] = true;
                            
                            // get output value for next iteration toggle
                            nvm_cfg.flags.out_read_value[i] = gpio_get_level(output_pins[i]);
                            // ESP_LOGE("TEST", "nvm_cfg.flags.out_read_value[%d] = %d", i, nvm_cfg.flags.out_read_value[i]);
                            // ESP_LOGW("TEST REPEAT", "set_time_in_ms: %llu | interval_in_ms: %lu | current_time_ms: %llu", cfg.timers[0].set_time_in_ms, cfg.timers[0].interval_in_ms, nvm_cfg.flags.current_time_ms);
                        }
                        else {
                            nvm_cfg.flags.rep_out[i] = false;
                        }
                    }
                }
            }
        }

        // raise current time in ms by one as we have finished all for this ms run
        nvm_cfg.flags.current_time_ms++;

        // wait until 1 ms has passed in this task (so the time it took for all code to run in that ms and the fillin until a full ms has passed)
        xTaskDelayUntil(&lastUpdateTime, 1 / portTICK_PERIOD_MS);
    }
}




// void updateElapsedTimeTask(void *pvParameters) {
//     // define function variables
//     uint16_t elapsedTimeMillis = 0;             // global variable to store elapsed time
//     struct timeval lastUpdateTime;              // declare a variable to store the last update time
//     gettimeofday(&lastUpdateTime, NULL);        // get the current time and store it in the lastUpdateTime variable when first running this (only once)
//     static uint16_t elapsedTimeCounter = 100;   // declare a counter to keep track of elapsed time
//     static uint16_t elapsedTimeCounter_1ms = 1; // declare a counter to keep track of elapsed time in ms

//     // infinite while loop in task
//     while (1) {
//         struct timeval currentTime;
//         gettimeofday(&currentTime, NULL);

//         // calculate elapsed time in milliseconds
//         elapsedTimeMillis = (uint64_t)(currentTime.tv_sec - lastUpdateTime.tv_sec) * 1000 +
//                             (uint64_t)(currentTime.tv_usec - lastUpdateTime.tv_usec) / 1000;

//         if (elapsedTimeMillis >= elapsedTimeCounter_1ms) {
//             // check each ms all timers against system time
//             check_timers_time_to_system_time(elapsedTimeMillis);
//             elapsedTimeCounter_1ms += 1;
//         }

//         // LOOP TASK
//         // check if 100 milliseconds have passed to update display
//         if (elapsedTimeMillis >= elapsedTimeCounter) {
//             // update display every 100 milliseconds
//             xTaskCreatePinnedToCore(Display_ssd1306, "Display_ssd1306", 4096*2, NULL, 6, NULL, 0);
//             // reset the counter only if it's greater than or equal to 100
//             elapsedTimeCounter += 100;
//         }

//         // reset vars at the end of each second
//         if (elapsedTimeMillis >= 1000) {
//             // Reset the counter
//             elapsedTimeCounter = 0;
//             lastUpdateTime = currentTime;
//             elapsedTimeMillis = 0;
//             elapsedTimeCounter_1ms = 0;

//             if (nvm_cfg.flags.clock_flag == false) {
//                 // set system time to cfg
//                 read_system_time_to_cfg();
//             }
//         }
//     }
// }

// // check if timer moment has arrived
// void check_timers_time_to_system_time(uint16_t counting_ms) {
//     // loop over each timer
//     for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
//         // check if timer moment is here
//         if (cfg.timers[i].set_day == nvm_cfg.rtc.day &&
//             cfg.timers[i].set_hour == nvm_cfg.rtc.hour &&
//             cfg.timers[i].set_min == nvm_cfg.rtc.min &&
//             cfg.timers[i].set_sec == nvm_cfg.rtc.sec) {
//             if (cfg.timers[i].set_ms == counting_ms) {
//                 if ((cfg.timers[i].repeat_timer != 0) && (cfg.timers[i].setting_timer_output_useonce == false)) {
//                     // set timer output regularly only once, next cycle is repeat
//                     set_timer_output(i);

//                     // call function to set repeat timer
//                     xTaskCreatePinnedToCore(timer_start_periodic, "timer_start_periodic", 4096*16, i, 6, NULL, 0);

//                     // set flag to true
//                     cfg.timers[i].setting_timer_output_useonce = true;
//                 } else if (cfg.timers[i].repeat_timer == 0){
//                     // set output
//                     set_timer_output(i);
//                 }
//             }
//         }
//     }
// }

// // set timer output when timer moment has arrived
// void set_timer_output(uint8_t timer_number) {
//     // check if timer is active
//     if (cfg.timers[timer_number].timer_active == true) {
//         ESP_LOGE("TEST", "[%d] timer output set to [%d]", timer_number, cfg.timers[timer_number].set_value);
//         gpio_set_level(output_pins[timer_number], cfg.timers[timer_number].set_value);
//     }
// }

// // timer callback function
// void timer_callback(void* arg) {
//     // Cast the argument to the appropriate type
//     int timer_index = (int)arg;

//     // check if the repeat_timer is set
//     if (cfg.timers[timer_index].repeat_timer == 0) {
//         // stop the timer if the flag is set
//         esp_timer_stop(periodic_timers[timer_index]);

//         // make sure the flag is set to false so we can do our first run again
//         cfg.timers[timer_index].setting_timer_output_useonce = false;
        
//         // exit the callback
//         return;
//     }
    
//     // toggle the specified output
//     gpio_set_level(output_pins[timer_index], !gpio_get_level(output_pins[timer_index]));
// }

// // function to start a periodic timer
// void timer_start_periodic(int timer_index) {
//     ESP_LOGE("TEST", "starting timer_start_periodic");

//     // Check if the timer with the same index already exists, stop and delete it
//     if (periodic_timers[timer_index] != NULL) {
//         esp_err_t stop_err = esp_timer_stop(periodic_timers[timer_index]);
//         ESP_ERROR_CHECK(stop_err);

//         esp_err_t delete_err = esp_timer_delete(periodic_timers[timer_index]);
//         ESP_ERROR_CHECK(delete_err);
//     }

//     // create timer arguments
//     const esp_timer_create_args_t timer_args = {
//         .callback = &timer_callback,
//         .arg = (void*)timer_index,
//         .name = "periodic_timer",
//         .dispatch_method = ESP_TIMER_ISR
//     };

//     ESP_LOGE("TEST", "creating timer for repeating");

//     // create repeat timer
//     ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timers[timer_index]));

//     ESP_LOGE("TEST", "starting timer for repeating");

//     // start the repeat timer
//     // ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timers[timer_index], cfg.timers[timer_index].interval_in_ms*1000));
//     ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timers[timer_index], 1000));      // "TEST" set back when done testing

//     // task is done, delete itself
//     vTaskDelete(NULL);
// }

// callback to set all timer outputs (repeat all functionality)
void timer_all_callback() {
    // check if the repeat_timer is set
    if (cfg.timers[0].repeat_timer == 1) {
        // stop all timers when setting first timer to no repeat
        esp_timer_stop(periodic_timers_all);

        // feedback
        ESP_LOGI(MS_TIMER_TAG, "Timer [0] set to no repeat, turning off repeat all functionality");
        
        // exit the callback
        return;
    }
    
    // toggle all outputs
    gpio_set_level(output_pins[0], !gpio_get_level(output_pins[0]));
    gpio_set_level(output_pins[1], !gpio_get_level(output_pins[1]));
    gpio_set_level(output_pins[2], !gpio_get_level(output_pins[2]));
    gpio_set_level(output_pins[3], !gpio_get_level(output_pins[3]));
}

// function to start repeat for all timers (repeat all functionality)
void timer_start_periodic_all() {
    // feedback
    ESP_LOGI(MS_TIMER_TAG, "Starting repeat all timers functionality");

    // create timer arguments
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_all_callback,
        .name = "periodic_timer_all",
        .dispatch_method = ESP_TIMER_TASK
    };

    // set all repeat timer flags to 0, timers cannot repeat when using this function!
    for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
        cfg.timers[i].repeat_timer = 0;
    }

// "TEST", if already called over CLI once, delete that timer and create a new one with new ms interval.
    // create repeat timer
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timers_all));

    // start the repeat timer
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timers_all, cfg.timers[0].interval_in_ms*1000));
}