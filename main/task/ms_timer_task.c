// includes
#include "task.h"

#define MS_TIMER_TAG "MS_TIMER"

// periodic timers (NOT actual cfg timers!)
esp_timer_handle_t periodic_timers[MAX_TIMER_COUNT];

esp_timer_handle_t periodic_timers_all;

gpio_num_t output_pins[4] = {OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4};

// declare all ms timer functions in here
// function to set outputs and check next iteration each ms (task based)
// void updateElapsedTimeTask(void *pvParameters) {
//     // define function variables
//     TickType_t lastUpdateTime = xTaskGetTickCount(); // get current time in ticks and save (at start of task only)

//     uint8_t set_out[4];
//     uint8_t rep_out[4];
//     uint8_t out_read_value[4];
//     uint8_t first_run_done[4];

//     const gpio_num_t output_pins[4] = {OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4};

//     // calculate ms time from start of week (rtc time)
//     nvm_cfg.flags.current_time_ms = (uint64_t)nvm_cfg.rtc.day * 86400000 +       // "TEST" current_time_ms needs to reset to 0 when week in counting has finished and a new week (00:00:00:00:000) has begun
//                                 (uint64_t)nvm_cfg.rtc.hour * 3600000 +
//                                 (uint64_t)nvm_cfg.rtc.min * 60000 +
//                                 (uint64_t)nvm_cfg.rtc.sec * 1000;

//     // infinite while loop in task
//     while (1) {
//         // set schakelmoment outputs based on previously calculated values
//         // offering clean code for faster switching of outputs
//         if (set_out[0] == true) {
//             gpio_set_level(OUTPUT_1, 1);
//         }
//         if (set_out[1] == true) {
//             gpio_set_level(OUTPUT_2, 1);
//         }
//         if (set_out[2] == true) {
//             gpio_set_level(OUTPUT_3, 1);
//         }
//         if (set_out[3] == true) {
//             gpio_set_level(OUTPUT_4, 1);
//         }

//         // toggle outputs if repeat moment arrived
//         if (rep_out[0] == true) {
//             gpio_set_level(OUTPUT_1, !out_read_value[0]);
//         }
//         if (rep_out[1] == true) {
//             gpio_set_level(OUTPUT_2, !out_read_value[1]);
//         }
//         if (rep_out[2] == true) {
//             gpio_set_level(OUTPUT_3, !out_read_value[2]);
//         }
//         if (rep_out[3] == true) {
//             gpio_set_level(OUTPUT_4, !out_read_value[3]);
//         }

//         // calculate values for next iteration
//         for (uint8_t i = 0; i <= 3; i++) {
//             if (cfg.timers[i].timer_active == 1) {
//                 // has set time been reached?
//                 if ((cfg.timers[i].set_time_in_ms+1 == nvm_cfg.flags.current_time_ms+1) && (first_run_done[i] == false)) {
//                     set_out[i] = true;
//                     first_run_done[i] = true;
//                 }
//                 // no set time, check repeat time if set has been done at least once
//                 else {
//                     set_out[i] = false;
//                     // did we stop timer from repeating?
//                     if (cfg.timers[i].repeat_timer == 0) {
//                         first_run_done[i] = false;
//                     }
//                     // no set time, are we still repeating and has set been done once?
//                     if (first_run_done[i] == true) {
//                         // check if timer is repeat (calculate time passed since set moment, divide it by repeat_interval_ms using modulus to get the remaining value. If it is not equal to 0, next iteration is not a repeat moment)
//                         if ((((nvm_cfg.flags.current_time_ms+1) - (cfg.timers[i].set_time_in_ms+1)) % cfg.timers[i].interval_in_ms) == 0) {
//                             rep_out[i] = true;
                            
//                             // get output value for next iteration toggle
//                             out_read_value[i] = gpio_get_level(output_pins[i]);
//                         }
//                         else {
//                             rep_out[i] = false;
//                         }
//                     }
//                 }
//             }
//         }

//         // raise current time in ms by one as we have finished all for this ms run
//         nvm_cfg.flags.current_time_ms++;

//         if (nvm_cfg.rtc.day >= 8) {
//             ESP_LOGE("TEST", "day became greater than 8, setting to modulus of 7");
//             nvm_cfg.rtc.day = nvm_cfg.rtc.day % 7;
//             // set system time in ms for synchronisation in output checks (always follow system time)
//             nvm_cfg.flags.current_time_ms = (uint64_t)1 * 86400000 +
//                                         (uint64_t)nvm_cfg.rtc.hour * 3600000 +
//                                         (uint64_t)nvm_cfg.rtc.min * 60000 +
//                                         (uint64_t)nvm_cfg.rtc.sec * 1000;
//         }

//         // wait until 1 ms has passed in this task (so the time it took for all code to run in that ms and the fillin until a full ms has passed)
//         xTaskDelayUntil(&lastUpdateTime, 1 / portTICK_PERIOD_MS);
//     }
// }

// function to init esp timer to interrupt each ms and set outputs
void init_ms_outputs_intr() {
    const esp_timer_create_args_t timer_args = {
        .callback = &set_outputs_intr_callback,
        .arg = NULL,
        .name = "outputs_intr_timer",
        .skip_unhandled_events = true,
        .dispatch_method = ESP_TIMER_ISR
    };

    // create the timer
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);

    // start the timer (1000 us = 1 ms)
    esp_timer_start_periodic(timer, 1000);
}

// set outputs based on given data from timers (interrupt based)
void set_outputs_intr_callback() {
    // set schakelmoment outputs based on previously calculated values
    // offering clean code for faster switching of outputs
    if (nvm_cfg.flags.set_out[0] == true) {
        gpio_set_level(OUTPUT_1, 1);
    }
    if (nvm_cfg.flags.set_out[1] == true) {
        gpio_set_level(OUTPUT_2, 1);
    }
    if (nvm_cfg.flags.set_out[2] == true) {
        gpio_set_level(OUTPUT_3, 1);
    }
    if (nvm_cfg.flags.set_out[3] == true) {
        gpio_set_level(OUTPUT_4, 1);
    }

    // toggle outputs if repeat moment arrived
    if (nvm_cfg.flags.rep_out[0] == true) {
        gpio_set_level(OUTPUT_1, !nvm_cfg.flags.out_read_value[0]);
    }
    if (nvm_cfg.flags.rep_out[1] == true) {
        gpio_set_level(OUTPUT_2, !nvm_cfg.flags.out_read_value[1]);
    }
    if (nvm_cfg.flags.rep_out[2] == true) {
        gpio_set_level(OUTPUT_3, !nvm_cfg.flags.out_read_value[2]);
    }
    if (nvm_cfg.flags.rep_out[3] == true) {
        gpio_set_level(OUTPUT_4, !nvm_cfg.flags.out_read_value[3]);
    }

    // calculate values for next iteration
    for (uint8_t i = 0; i <= 3; i++) {
        if (cfg.timers[i].timer_active == 1) {
            // has set time been reached?
            if ((cfg.timers[i].set_time_in_ms+1 == nvm_cfg.flags.current_time_ms+1) && (nvm_cfg.flags.first_run_done[i] == false)) {
                nvm_cfg.flags.set_out[i] = true;
                nvm_cfg.flags.first_run_done[i] = true;
            }
            // no set time, check repeat time if set has been done at least once
            else {
                nvm_cfg.flags.set_out[i] = false;
                // did we stop timer from repeating?
                if (cfg.timers[i].repeat_timer == 0) {
                    nvm_cfg.flags.first_run_done[i] = false;
                }
                // no set time, are we still repeating and has set been done once?
                if (nvm_cfg.flags.first_run_done[i] == true) {
                    // check if timer is repeat (calculate time passed since set moment, divide it by repeat_interval_ms using modulus to get the remaining value. If it is not equal to 0, next iteration is not a repeat moment)
                    if ((((nvm_cfg.flags.current_time_ms+1) - (cfg.timers[i].set_time_in_ms+1)) % cfg.timers[i].interval_in_ms) == 0) {
                        nvm_cfg.flags.rep_out[i] = true;

                        // get output value for next iteration toggle
                        nvm_cfg.flags.out_read_value[i] = gpio_get_level(output_pins[i]);
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

    if (nvm_cfg.rtc.day >= 8) {
        nvm_cfg.rtc.day = nvm_cfg.rtc.day % 7;
        // set system time in ms for synchronisation in output checks (always follow system time)
        nvm_cfg.flags.current_time_ms = (uint64_t)1 * 86400000 +
                                    (uint64_t)nvm_cfg.rtc.hour * 3600000 +
                                    (uint64_t)nvm_cfg.rtc.min * 60000 +
                                    (uint64_t)nvm_cfg.rtc.sec * 1000;
    }
}