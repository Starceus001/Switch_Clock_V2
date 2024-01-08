// includes
#include "task.h"

#define BUTTON_TAG "BUTTONS"

// Semaphore for synchronizing access to shared resources (e.g., flags)
SemaphoreHandle_t buttonSemaphore;

// define array with all buttons
gpio_num_t digital_buttons[NUM_DIGITAL_BUTTONS] = {
    DAG_KNOP,
    TIMER_KNOP,
    TIMER_ACTIEF_KNOP,
    SCHAKELUITGANG_AANUIT_KNOP,
    HERHAALSCHAKELMOMENT_KNOP
};

gpio_num_t analog_buttons[NUM_ANALOG_BUTTONS] = {
    CLOCK_KNOP,
    UUR_KNOP,
    MINUUT_KNOP,
    SECONDEN_KNOP,
    MSCENONDE_KNOP
};

// declare all button functions in here
void handle_button_press(gpio_num_t buttonPin) {
    // Print the button pin number
    ESP_LOGI("TEST", "[%d] Button pressed", buttonPin);

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
    gpio_num_t buttonPin = (gpio_num_t) arg;

    // Ensure we have exclusive access to the button handling logic and that the semaphore has 100ms to be available
    if (xSemaphoreTake(buttonSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
        // Handle the button press
        handle_button_press(buttonPin);

        // Release the semaphore
        xSemaphoreGive(buttonSemaphore);
    }

    // Task is done, delete itself
    vTaskDelete(NULL);
}

void init_gpio() {
    // feedback
    ESP_LOGI(BUTTON_TAG, "Initialise GPIO");

// DIGITAL INPUTS -----------------------------------------------------------------------------------
    // configure digital buttons as inputs with pull-down and interrupts
    gpio_config_t input_conf = {
        .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };

    // install isr service (once)
    gpio_install_isr_service(0);

    // run gpio config for all defined buttons
    gpio_config(&input_conf);

    // add isr handler to each button pin
    for (int i = 0; i < (NUM_DIGITAL_BUTTONS); i++) {
        gpio_isr_handler_add(digital_buttons[i], digital_button_isr_handler, (void*)digital_buttons[i]);
    }

// ANALOG INPUTS -----------------------------------------------------------------------------------
    // Configure analog input pins
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(CLOCK_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(UUR_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MINUUT_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(SECONDEN_KNOP, ADC_ATTEN_DB_0);
    adc2_config_channel_atten(MSCENONDE_KNOP, ADC_ATTEN_DB_0);

    // create task to constantly check analog inputs (buttons)
    xTaskCreate(analog_button_check_task, "analog_button_check_task", 2048, NULL, 5, NULL);

// DIGITAL OUTPUTS -----------------------------------------------------------------------------------
    // Configure digital output pins as outputs
    gpio_config_t digital_output_conf = {
        .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    // config gpio for outputs
    gpio_config(&digital_output_conf);

// FINISHERS ----------------------------------------------------------------------------------------
    // Create a binary semaphore for synchronizing access to shared resources
    buttonSemaphore = xSemaphoreCreateBinary();

    // feedback
    ESP_LOGI(BUTTON_TAG, "Initialise GPIO done");
}

void digital_button_isr_handler(void* arg) {
    gpio_num_t buttonPin = (gpio_num_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Notify the task that a button was pressed
    if (xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken) == pdTRUE) {
        // If using FreeRTOS, yield to the task if a higher-priority task was woken
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else {
        // Create a task to handle the button press
        xTaskCreate(button_handle_task, "button_handle_task", 2048, (void*)buttonPin, 5, NULL);
    }
}

// task that continuously monitors analog pins
void analog_button_check_task(void* arg) {
    uint8_t adc_value;
    TickType_t lastPressTime[NUM_ANALOG_BUTTONS] = {0};

    while (1) {
        // check analog pins for button presses
        for (int i = 0; i < NUM_ANALOG_BUTTONS; i++) {
            gpio_num_t buttonPin = analog_buttons[i];

            // if MS_KNOP, do ADC2 reading, all else is ADC1 reading
            if (buttonPin == 3) {
                // check if it is ADC1 or ADC2 (MS_KNOP is last in list so number 4 is ADC2)
                if (i == 4) {
                    // use ADC2 for button MS_KNOP on pin 3
                    adc2_get_raw(buttonPin, ADC_WIDTH_BIT_12, &adc_value);
                // not last one in list so it is button UUR_KNOP as ADC1
                } else {
                    // use ADC1 for other buttons on pin 3
                    adc_value = adc1_get_raw(buttonPin);
                }
            // it is not pin 3 so all others are ADC1
            } else {
                // use ADC1 for buttons on other pins
                adc_value = adc1_get_raw(buttonPin);
            }

            // check if button press is correct with threshold and debouncing
            if (adc_value > ANALOG_THRESHOLD) {
                // check if enough time has passed since the last press
                TickType_t currentTime = xTaskGetTickCount();
                if ((currentTime - lastPressTime[i]) > pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
                    // is CLOCK_KNOP button pressed? (only valid if last press was at least 1 sec ago)
                    if (i == 0) {
                        if ((currentTime - lastPressTime[0]) > pdMS_TO_TICKS(1000)) {
                            // analog button press detected, create a task to handle it
                            xTaskCreate(button_handle_task, "button_handle_task", 2048, (void*)buttonPin, 5, NULL);

                            // update the last press time
                            lastPressTime[i] = currentTime;
                        }
                    }
                    // all other buttons
                    else {
                        // analog button press detected, create a task to handle it
                        xTaskCreate(button_handle_task, "button_handle_task", 2048, (void*)buttonPin, 5, NULL);

                        // update the last press time
                        lastPressTime[i] = currentTime;
                    }
                }
            }
        }

        // introduce a delay to avoid constant checking and reduce CPU usage
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// BUTTON FUNCTIONS
// ---------------------------------------------------------------------------------------------------------------------------------------------
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

