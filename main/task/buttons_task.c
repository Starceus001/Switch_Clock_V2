// includes
#include "task.h"

#define BUTTON_TAG "BUTTONS"

// Semaphore for synchronizing access to shared resources (e.g., flags)
SemaphoreHandle_t buttonSemaphore;

// define array with all buttons
gpio_num_t all_buttons[NUM_DIGITAL_BUTTONS + NUM_ANALOG_BUTTONS] = {
    DAG_KNOP,
    TIMER_KNOP,
    TIMER_ACTIEF_KNOP,
    SCHAKELUITGANG_AANUIT_KNOP,
    HERHAALSCHAKELMOMENT_KNOP,
    CLOCK_KNOP,
    UUR_KNOP,
    MINUUT_KNOP,
    SECONDEN_KNOP,
    MSCENONDE_KNOP
};

// declare all button functions in here
void handle_button_press(gpio_num_t buttonPin) {
    // Print the button pin number
    ESP_LOGI("TEST", "Button on pin %d pressed", buttonPin);

    // "TEST" use this function when all button reading works to do things with the flags after a flag set (like updating a clock var with the time buttons or updating the display if chosen not to do that in the main loop)
    // switch (buttonPin) {
    //     case DAG_KNOP:
    //         // Handle DAG_KNOP button press
    //         DAG_KNOP_button_pressed();
    //         break;
    //     case TIMER_KNOP:
    //         // Handle TIMER_KNOP button press
    //         TIMER_KNOP_button_pressed();
    //         break;
    //     case TIMER_ACTIEF_KNOP:
    //         // Handle TIMER_ACTIEF_KNOP button press
    //         TIMER_ACTIEF_KNOP_button_pressed();
    //         break;
    //     case SCHAKELUITGANG_AANUIT_KNOP:
    //         // Handle SCHAKELUITGANG_AANUIT_KNOP button press
    //         SCHAKELUITGANG_AANUIT_KNOP_button_pressed();
    //         break;
    //     case HERHAALSCHAKELMOMENT_KNOP:
    //         // Handle HERHAALSCHAKELMOMENT_KNOP button press
    //         HERHAALSCHAKELMOMENT_KNOP_button_pressed();
    //         break;
    //     case CLOCK_KNOP:
    //         // Handle CLOCK_KNOP button press
    //         CLOCK_KNOP_button_pressed();
    //         break;
    //     case UUR_KNOP:
    //         // Handle UUR_KNOP button press
    //         UUR_KNOP_button_pressed();
    //         break;
    //     case MINUUT_KNOP:
    //         // Handle MINUUT_KNOP button press
    //         MINUUT_KNOP_button_pressed();
    //         break;
    //     case SECONDEN_KNOP:
    //         // Handle SECONDEN_KNOP button press
    //         SECONDEN_KNOP_button_pressed();
    //         break;
    //     // case MSCENONDE_KNOP:     // "TEST" look into why this button pin has the same value as UUR_KNOP button pin
    //     //     // Handle MSCENONDE_KNOP button press
    //     //     MSCENONDE_KNOP_button_pressed();
    //     //     break;
    //     default:
    //         // Handle unknown button press
    //         ESP_LOGE(BUTTON_TAG, "Unknown button press, doing nothing");
    //         break;
    // }
}

void button_handle_task(void* arg) {
    ESP_LOGE("TEST", "entered button_handle_task"); // "TEST"
    gpio_num_t buttonPin = (gpio_num_t) arg;

    // Ensure we have exclusive access to the button handling logic
    if (xSemaphoreTake(buttonSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
        ESP_LOGE("TEST", "semaphore available, start handle_button_press"); // "TEST"
        // Handle the button press
        handle_button_press(buttonPin);

        // Release the semaphore
        xSemaphoreGive(buttonSemaphore);
    }

    ESP_LOGE("TEST", "button_handle_task done, deleting task"); // "TEST"
    // Task is done, delete itself
    vTaskDelete(NULL);
}

// older testcode provided by chatgpt
// void init_gpio() {
//     // feedback
//     ESP_LOGI(BUTTON_TAG, "Initialize GPIO");

//     // Configure digital output pins as outputs
//     gpio_config_t digital_output_conf = {
//         .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
//         .mode = GPIO_MODE_OUTPUT,
//         .intr_type = GPIO_INTR_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//     };

//     // config GPIO for outputs
//     gpio_config(&digital_output_conf);

//     // Configure input pins as inputs with interrupts
//     gpio_config_t input_conf = {
//         // .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP),
//         .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP) | (1ULL << CLOCK_KNOP) | (1ULL << UUR_KNOP) | (1ULL << MINUUT_KNOP) | (1ULL << SECONDEN_KNOP) | (1ULL << MSCENONDE_KNOP),
//         .mode = GPIO_MODE_INPUT,
//         .intr_type = GPIO_INTR_POSEDGE,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_ENABLE,
//     };

//     ESP_LOGE("TEST", "Start gpio_config for all buttons");
//     // Run GPIO config for all buttons after configuring ISR handlers
//     gpio_config(&input_conf);


//     ESP_LOGE("TEST", "Start gpio_install_isr_service");
//     // Install ISR service (once)
//     gpio_install_isr_service(0);

//     // Run GPIO config for all defined buttons
//     for (int i = 0; i < NUM_DIGITAL_BUTTONS + NUM_ANALOG_BUTTONS; i++) {
//         ESP_LOGE("TEST", "Looping for loop %d to set gpio for all defined buttons", i);
//         input_conf.pin_bit_mask = (1ULL << all_buttons[i]);
//         gpio_isr_handler_add(all_buttons[i], button_isr_handler, (void*)all_buttons[i]);
//     }

//     // Create a binary semaphore for synchronizing access to shared resources
//     buttonSemaphore = xSemaphoreCreateBinary();

//     // feedback
//     ESP_LOGI(BUTTON_TAG, "Initialize GPIO done");
// }



// newest test code
// void init_gpio() {
//     // feedback
//     ESP_LOGI(BUTTON_TAG, "Initialise GPIO");

//     // Configure digital output pins as outputs
//     gpio_config_t digital_output_conf = {
//         .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
//         .mode = GPIO_MODE_OUTPUT,
//         .intr_type = GPIO_INTR_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//     };
//     // ESP_LOGE("TEST", "Start gpio_config");
//     // config gpio for outputs
//     gpio_config(&digital_output_conf);

//     // Configure input pins as inputs with interrupts
//     gpio_config_t input_conf = {
//         // .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP),
//         .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP) | (1ULL << CLOCK_KNOP) | (1ULL << UUR_KNOP) | (1ULL << MINUUT_KNOP) | (1ULL << SECONDEN_KNOP) | (1ULL << MSCENONDE_KNOP),
//         .mode = GPIO_MODE_INPUT,
//         .intr_type = GPIO_INTR_POSEDGE,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_ENABLE,
//     };

//     ESP_LOGE("TEST", "Start gpio_install_isr_service");
//     // install isr service (once)
//     gpio_install_isr_service(1);



//     ESP_LOGE("TEST", "configure analog input pins?");
//     // Configure analog input pins
//     adc1_config_width(ADC_WIDTH_BIT_12);
//     adc1_config_channel_atten(CLOCK_KNOP, ADC_ATTEN_DB_0);
//     adc1_config_channel_atten(UUR_KNOP, ADC_ATTEN_DB_0);
//     adc1_config_channel_atten(MINUUT_KNOP, ADC_ATTEN_DB_0);
//     adc1_config_channel_atten(SECONDEN_KNOP, ADC_ATTEN_DB_0);
//     adc1_config_channel_atten(MSCENONDE_KNOP, ADC_ATTEN_DB_0);


//     ESP_LOGE("TEST", "run gpio_config for all defined buttons");
//     // run gpio config for all defined buttons
//     gpio_config(&input_conf);
//     ESP_LOGE("TEST", "gpio_config done");

//     // Run GPIO config for all defined buttons
//     for (int i = 0; i < NUM_DIGITAL_BUTTONS + NUM_ANALOG_BUTTONS; i++) {
//         ESP_LOGE("TEST", "Looping for loop %d to set gpio for all defined buttons", i);
//         input_conf.pin_bit_mask = (1ULL << all_buttons[i]);
//         gpio_isr_handler_add(all_buttons[i], button_isr_handler, (void*)all_buttons[i]);
//         ESP_LOGE("TEST", "gpio_isr_handler_add done");
//     }

//     ESP_LOGE("TEST", "All done, gonna run xSemaphoreCreateBinary");

//     // Create a binary semaphore for synchronizing access to shared resources
//     buttonSemaphore = xSemaphoreCreateBinary();

//     // feedback
//     ESP_LOGI(BUTTON_TAG, "Initialise GPIO done");
// }






// my code that works for 5 buttons but crashes after button press (might be fixed with tasks now??)
void init_gpio() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Initialise GPIO");

    gpio_config_t input_conf = {
        .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP),
        // .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP) | (1ULL << CLOCK_KNOP) | (1ULL << UUR_KNOP) | (1ULL << MINUUT_KNOP) | (1ULL << SECONDEN_KNOP) | (1ULL << MSCENONDE_KNOP),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };

    ESP_LOGE("TEST", "Start gpio_install_isr_service");
    // install isr service (once)
    gpio_install_isr_service(1);

    // Configure analog input pins
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(CLOCK_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(UUR_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MINUUT_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(SECONDEN_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MSCENONDE_KNOP, ADC_ATTEN_DB_0);

    ESP_LOGE("TEST", "Start gpio_config(&input_conf);");
    // run gpio config for all defined buttons
    gpio_config(&input_conf);
    ESP_LOGE("TEST", "finished gpio_config(&input_conf);");

    // add isr handler to each button pin
    for (int i = 0; i <= (NUM_DIGITAL_BUTTONS + NUM_ANALOG_BUTTONS); i++) {
        ESP_LOGE("TEST", "Looping for loop %d", i);
        input_conf.pin_bit_mask = (1ULL << all_buttons[i]);
        gpio_isr_handler_add(all_buttons[i], button_isr_handler, (void*)all_buttons[i]);
        ESP_LOGE("TEST", "gpio_isr_handler_add done");
    }
    
    ESP_LOGE("TEST", "Start output config (look at the prints above, did we get all the 10 buttons this time?)");

    // Configure digital output pins as outputs
    gpio_config_t digital_output_conf = {
        .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_LOGE("TEST", "Start gpio_config(&digital_output_conf);");
    // config gpio for outputs
    gpio_config(&digital_output_conf);

    ESP_LOGE("TEST", "All done, gonna run xSemaphoreCreateBinary");

    // Create a binary semaphore for synchronizing access to shared resources
    buttonSemaphore = xSemaphoreCreateBinary();

    // feedback
    ESP_LOGI(BUTTON_TAG, "Initialise GPIO done");
}

void button_isr_handler(void* arg) {
    ESP_LOGE("TEST", "entered button_isr_handler"); // "TEST"

    gpio_num_t buttonPin = (gpio_num_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // feedback
    // ESP_LOGI(BUTTON_TAG, "Button %d pressed, running button functionality with semaphore and writing button flag", buttonPin);

    // Notify the task that a button was pressed
    if (xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken) == pdTRUE) {
        // Create a task to handle the button press
        xTaskCreate(button_handle_task, "button_handle_task", 2048, (void*)buttonPin, 5, NULL);
    }

    // If using FreeRTOS, yield to the task if a higher-priority task was woken
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// void button_isr_handler(void* arg) {
//     ESP_LOGE("TEST", "entered button_isr_handler"); // "TEST"

//     gpio_num_t buttonPin = (gpio_num_t)arg;
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//     // feedback
//     // ESP_LOGI(BUTTON_TAG, "Button %d pressed, running button functionality with semaphore and writing button flag", buttonPin);

//     // Set the flag corresponding to the pressed button
//     // handle_button_press(buttonPin);

//     // Notify the task that a button was pressed
//     xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);

//     // If using FreeRTOS, yield to the task if a higher-priority task was woken
//     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
// }































































 
void DAG_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start DAG_KNOP functionality");

    // are we writing for clock?
    if (nvm_cfg.flags.clock_flag == 1) {
        // clock is active, did we reach end of day cycle?
        if (nvm_cfg.rtc.day == 7) {
            ESP_LOGI(BUTTON_TAG, "Clock day limit reached (7), resetting to 1");
            nvm_cfg.rtc.day = 1;
        }
        // up day by one
        else {
            ESP_LOGI(BUTTON_TAG, "Raising day for clock by one to %d", nvm_cfg.rtc.day + 1);
            nvm_cfg.rtc.day++;
        }
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we not repeating this timer? (not needed for the timer repeat interval as it is less than 1 day only)
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer != 1) {
            // did we reach end of day cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_day == 7) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment day limit reached (7), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_day = 1;
            }
            // up day by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising day for timer setmoment by one to %d", nvm_cfg.rtc.day + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_day++;
            }
        }
        else {
            ESP_LOGI(BUTTON_TAG, "Timer repeat interval is active, no day change allowed");
        }
    }
    else {
        // no clock or timer active, do nothing
        ESP_LOGW(BUTTON_TAG, "No clock or timer active for day button, doing nothing");
    }
    return;
}

void TIMER_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start TIMER_KNOP functionality");

    // are we doing anything with the clock now?
    if (nvm_cfg.flags.clock_flag == 0) {
        // no clock actions, allowed to do timer stuff
        // first entry on this cycle?
        if (nvm_cfg.flags.timer_flag == 0) {
            nvm_cfg.flags.clock_flag = 0;           // could be unnecessary but for good measure
            nvm_cfg.flags.timer_flag = 1;
            nvm_cfg.flags.chosen_timer = 0;
        } else {
            if (nvm_cfg.flags.chosen_timer < MAX_TIMER_COUNT) {
                nvm_cfg.flags.chosen_timer++;
            } else {
                nvm_cfg.flags.timer_flag = 0;
                nvm_cfg.flags.chosen_timer = 0;     // could be unnecessary but for good measure
            }
        }
        ESP_LOGI(BUTTON_TAG, "(Cycling timers) Timer [%d] where timer_flag is %d", nvm_cfg.flags.chosen_timer, nvm_cfg.flags.timer_flag);
    }

    return;
}

void TIMER_ACTIEF_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start TIMER_ACTIEF_KNOP functionality");

    // are we doing anything with the clock now?
    if (nvm_cfg.flags.clock_flag == 0) {
        // no clock actions, are timer actions allowed?
        if (nvm_cfg.flags.timer_flag == 1) {
            // switching flag based on known state
            if (cfg.timers[nvm_cfg.flags.chosen_timer].timer_active == 0) {
                cfg.timers[nvm_cfg.flags.chosen_timer].timer_active = 1;
            } else {
                cfg.timers[nvm_cfg.flags.chosen_timer].timer_active = 0;
            }
            ESP_LOGI(BUTTON_TAG, "Timer [%d] timer_active is set to %d", nvm_cfg.flags.chosen_timer, cfg.timers[nvm_cfg.flags.chosen_timer].timer_active);
        }
    }

    return;
}

void SCHAKELUITGANG_AANUIT_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start SCHAKELUITGANG_AANUIT_KNOP functionality");

    // are we doing anything with the clock now?
    if (nvm_cfg.flags.clock_flag == 0) {
        // no clock actions, are timer actions allowed?
        if (nvm_cfg.flags.timer_flag == 1) {
            // switching flag based on known state
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_value == 0) {
                cfg.timers[nvm_cfg.flags.chosen_timer].set_value = 1;
            } else {
                cfg.timers[nvm_cfg.flags.chosen_timer].set_value = 0;
            }
        }
        ESP_LOGI(BUTTON_TAG, "Timer [%d] set_value is set to %d", nvm_cfg.flags.chosen_timer, cfg.timers[nvm_cfg.flags.chosen_timer].set_value);
    }

    return;
}

void HERHAALSCHAKELMOMENT_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start HERHAALSCHAKELMOMENT_KNOP functionality");

    // are we doing anything with the clock now?
    if (nvm_cfg.flags.clock_flag == 0) {
        // no clock actions, are timer actions allowed?
        if (nvm_cfg.flags.timer_flag == 1) {
            // switching flag based on known state
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0) {
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 1;
            } else {
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 0;
            }
        }
        ESP_LOGI(BUTTON_TAG, "Timer [%d] repeat_timer is set to %d", nvm_cfg.flags.chosen_timer, cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer);
    }

    return;
}

void CLOCK_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start CLOCK_KNOP functionality");

    // are we doing anything with timers now?
    if (nvm_cfg.flags.timer_flag == 0) {
        // no timer actions, allowed to do clock stuff
        // switching flag based on known state
        if (nvm_cfg.flags.clock_flag == 0) {
            nvm_cfg.flags.chosen_timer = 0;     // could be unnecessary but for good measure
            nvm_cfg.flags.timer_flag = 0;       // could be unnecessary but for good measure
            nvm_cfg.flags.clock_flag = 1;
        } else {
            nvm_cfg.flags.clock_flag = 0;
        }
        ESP_LOGI(BUTTON_TAG, "Clock button gave %d", nvm_cfg.flags.clock_flag);
    }
    return;
}

void UUR_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start UUR_KNOP functionality");

    // are we writing for clock?
    if (nvm_cfg.flags.clock_flag == 1) {
        // clock is active, did we reach end of hour cycle?
        if (nvm_cfg.rtc.hour == 24) {
            ESP_LOGI(BUTTON_TAG, "Clock hour limit reached (24), resetting to 1");
            nvm_cfg.rtc.hour = 1;
        }
        // up hour by one
        else {
            ESP_LOGI(BUTTON_TAG, "Raising hour for clock by one to %d", nvm_cfg.rtc.hour + 1);
            nvm_cfg.rtc.hour++;
        }
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we repeating this timer?
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1) {
            // update timer repeat interval
            // did we reach end of hour cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour == 24) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat hour limit reached (24), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour = 1;
            }
            // up hour by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising hour for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour++;
            }
        }
        // change timer set time
        else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer != 1) {
            // did we reach end of hour cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_hour == 24) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment day limit reached (7), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_hour = 1;
            }
            // up hour by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising hour for timer setmoment by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].set_hour + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_hour++;
            }
        }
    }
    else {
        // no clock or timer active, do nothing
        ESP_LOGW(BUTTON_TAG, "No clock or timer active for hour button, doing nothing");
    }
    return;
}

void MINUUT_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start MINUUT_KNOP functionality");

    // are we writing for clock?
    if (nvm_cfg.flags.clock_flag == 1) {
        // clock is active, did we reach end of min cycle?
        if (nvm_cfg.rtc.min == 60) {
            ESP_LOGI(BUTTON_TAG, "Clock min limit reached (60), resetting to 1");
            nvm_cfg.rtc.min = 1;
        }
        // up min by one
        else {
            ESP_LOGI(BUTTON_TAG, "Raising min for clock by one to %d", nvm_cfg.rtc.min + 1);
            nvm_cfg.rtc.min++;
        }
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we repeating this timer?
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1) {
            // update timer repeat interval
            // did we reach end of min cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min == 60) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat min limit reached (60), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min = 1;
            }
            // up min by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising min for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min++;
            }
        }
        // change timer set time
        else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer != 1) {
            // did we reach end of min cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_min == 60) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment min limit reached (60), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_min = 1;
            }
            // up min by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising min for timer setmoment by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].set_min + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_min++;
            }
        }
    }
    else {
        // no clock or timer active, do nothing
        ESP_LOGW(BUTTON_TAG, "No clock or timer active for min button, doing nothing");
    }

    return;
}

void SECONDEN_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start SECONDEN_KNOP functionality");

    // are we writing for clock?
    if (nvm_cfg.flags.clock_flag == 1) {
        // clock is active, did we reach end of sec cycle?
        if (nvm_cfg.rtc.sec == 60) {
            ESP_LOGI(BUTTON_TAG, "Clock sec limit reached (60), resetting to 1");
            nvm_cfg.rtc.sec = 1;
        }
        // up sec by one
        else {
            ESP_LOGI(BUTTON_TAG, "Raising sec for clock by one to %d", nvm_cfg.rtc.sec + 1);
            nvm_cfg.rtc.sec++;
        }
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we repeating this timer?
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1) {
            // update timer repeat interval
            // did we reach end of sec cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec == 60) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat sec limit reached (60), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec = 1;
            }
            // up sec by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising sec for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec++;
            }
        }
        // change timer set time
        else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer != 1) {
            // did we reach end of sec cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_sec == 60) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment sec limit reached (60), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_sec = 1;
            }
            // up sec by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising sec for timer setmoment by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].set_sec + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_sec++;
            }
        }
    }
    else {
        // no clock or timer active, do nothing
        ESP_LOGW(BUTTON_TAG, "No clock or timer active for sec button, doing nothing");
    }

    return;
}

void MSCENONDE_KNOP_button_pressed() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Start MSCENONDE_KNOP functionality");

    // are we writing for clock?
    if (nvm_cfg.flags.clock_flag == 1) {
        ESP_LOGI(BUTTON_TAG, "Clock flag is active, no ms change allowed");
        return;
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we repeating this timer?
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1) {
            // update timer repeat interval
            // did we reach end of ms cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms == 999) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat ms limit reached (999), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms = 1;
            }
            // up ms by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising ms for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms++;
            }
        }
        // change timer set time
        else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer != 1) {
            // did we reach end of ms cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_ms == 999) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment ms limit reached (999), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_ms = 1;
            }
            // up ms by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising ms for timer setmoment by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].set_ms + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_ms++;
            }
        }
    }
    else {
        // no clock or timer active, do nothing
        ESP_LOGW(BUTTON_TAG, "No clock or timer active for ms button, doing nothing");
    }

    return;
}

