// includes
#include "task.h"

#define BUTTON_TAG "BUTTONS"

// Semaphore for synchronizing access to shared resources (e.g., flags)
SemaphoreHandle_t buttonSemaphore;

TickType_t lastButtonPressTime[NUM_DIGITAL_BUTTONS] = {0};

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
    ESP_LOGI(BUTTON_TAG, "[%d] Button pressed", buttonPin);

    switch (buttonPin) {
        case DAG_KNOP:
            // Handle DAG_KNOP button press
            DAG_KNOP_button_pressed();
            break;
        case TIMER_KNOP:
            // Handle TIMER_KNOP button press
            TIMER_KNOP_button_pressed();
            break;
        case TIMER_ACTIEF_KNOP:
            // Handle TIMER_ACTIEF_KNOP button press
            TIMER_ACTIEF_KNOP_button_pressed();
            break;
        case SCHAKELUITGANG_AANUIT_KNOP:
            // Handle SCHAKELUITGANG_AANUIT_KNOP button press
            SCHAKELUITGANG_AANUIT_KNOP_button_pressed();
            break;
        case HERHAALSCHAKELMOMENT_KNOP:
            // Handle HERHAALSCHAKELMOMENT_KNOP button press
            HERHAALSCHAKELMOMENT_KNOP_button_pressed();
            break;
        case CLOCK_KNOP:
            // Handle CLOCK_KNOP button press
            CLOCK_KNOP_button_pressed();
            break;
        case (UUR_KNOP+35):           // needed to make a difference between UUr knop and MS knop
            // Handle UUR_KNOP button press
            UUR_KNOP_button_pressed();
            break;
        case MINUUT_KNOP:
            // Handle MINUUT_KNOP button press
            MINUUT_KNOP_button_pressed();
            break;
        case SECONDEN_KNOP:
            // Handle SECONDEN_KNOP button press
            SECONDEN_KNOP_button_pressed();
            break;
        case (MSCENONDE_KNOP+36):     // needed to make a difference between UUr knop and MS knop
            // Handle MSCENONDE_KNOP button press
            MSCENONDE_KNOP_button_pressed();
            break;
        default:
            // Handle unknown button press
            ESP_LOGE(BUTTON_TAG, "[%d] Unknown button press, doing nothing", buttonPin);
            break;
    }
}

void button_handle_task(void* arg) {
    gpio_num_t buttonPin = (gpio_num_t) arg;

    // ensure we have exclusive access to the button handling logic and that the semaphore has 100ms to be available
    if (xSemaphoreTake(buttonSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
        // handle the button press
        handle_button_press(buttonPin);

        // release the semaphore
        xSemaphoreGive(buttonSemaphore);
    }

    // task is done, delete itself
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
    xTaskCreatePinnedToCore(analog_button_check_task, "analog_button_check_task", 2048, NULL, 1, NULL, 0);

// DIGITAL OUTPUTS -----------------------------------------------------------------------------------
    // Configure digital output pins as outputs
    gpio_config_t digital_output_conf = {
        .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
        .mode = GPIO_MODE_INPUT_OUTPUT,
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

// handle a digital button press interrupt and create task to handle the button
void digital_button_isr_handler(void* arg) {
    gpio_num_t buttonPin = (gpio_num_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // check if enough time has passed since the last button press
    uint32_t currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t lastPressTime = lastButtonPressTime[buttonPin];

    // debouncer for digital buttons
    if (currentTime - lastPressTime >= DIGITAL_DEBOUNCE_DELAY) {
        // Notify the task that a button was pressed
        if (xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken) == pdTRUE) {
            // If using FreeRTOS, yield to the task if a higher-priority task was woken
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else {
            // Create a task to handle the button press
            xTaskCreatePinnedToCore(button_handle_task, "button_handle_task", 4096, (void*)buttonPin, 1, NULL, 0);
            

            // update the last press time (debouncer for digital buttons)
            lastButtonPressTime[buttonPin] = currentTime;
        }
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
                if ((currentTime - lastPressTime[i]) > pdMS_TO_TICKS(ANALOG_DEBOUNCE_DELAY_MS)) {
                    // is CLOCK_KNOP button pressed? (only valid if last press was at least 1 sec ago)
                    if (i == 0) {
                        if ((currentTime - lastPressTime[0]) > pdMS_TO_TICKS(1000)) {
                            // analog button press detected, create a task to handle it
                            xTaskCreatePinnedToCore(button_handle_task, "button_handle_task", 4096, (void*)buttonPin, 1, NULL, 0);

                            // update the last press time
                            lastPressTime[i] = currentTime;
                        }
                    }
                    // seperate call for UUR_KNOP as it is on the same pin number as MS_KNOP, changing buttonPin for switch case
                    else if (i == 1) {
                        // analog button press detected, create a task to handle it (with special UUR_KNOP buttonPin value)
                        buttonPin = buttonPin+35;
                        xTaskCreatePinnedToCore(button_handle_task, "button_handle_task", 4096, (void*)buttonPin, 1, NULL, 0);
                    }
                    // seperate call for MS_KNOP as it is on the same pin number as UUR_KNOP, changing buttonPin for switch case
                    else if (i == 4) {
                        // analog button press detected, create a task to handle it (with special MS_KNOP buttonPin value)
                        buttonPin = buttonPin+36;
                        xTaskCreatePinnedToCore(button_handle_task, "button_handle_task", 4096, (void*)buttonPin, 1, NULL, 0);
                    }
                    // all other analog buttons
                    else {
                        // analog button press detected, create a task to handle it
                        xTaskCreatePinnedToCore(button_handle_task, "button_handle_task", 4096, (void*)buttonPin, 1, NULL, 0);
                    }
                    // update the last press time
                    lastPressTime[i] = currentTime;
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
        if (nvm_cfg.rtc.day >= 7) {
            ESP_LOGI(BUTTON_TAG, "Clock day limit reached (7), resetting to 1");
            nvm_cfg.rtc.day = 1;
        }
        // up day by one
        else {
            ESP_LOGI(BUTTON_TAG, "Raising day for clock by one to %d", nvm_cfg.rtc.day + 1);
            nvm_cfg.rtc.day++;
        }
        // update display day after day button press
        update_day_indicator(0, 0, 0);
    }
    // are we writing for timers?
    else if (nvm_cfg.flags.timer_flag == 1) {
        // timer is active, are we not repeating this timer? (not needed for the timer repeat interval as it is less than 1 day only)
        if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0) || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2)) {
            // did we reach end of day cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_day >= 7) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment day limit reached (7), resetting to 1");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_day = 1;
            }
            // up day by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising day for timer setmoment by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].set_day + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].set_day++;
            }
            // update display day after day button press
            update_day_indicator(2, 1, nvm_cfg.flags.chosen_timer);
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
            nvm_cfg.flags.clock_flag = 0;               // could be unnecessary but for good measure
            nvm_cfg.flags.timer_flag = 1;
            nvm_cfg.flags.chosen_timer = 0;
        } else {
            if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0) || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2)) {
                if (nvm_cfg.flags.chosen_timer < MAX_TIMER_COUNT) {
                    // reset flags for timer repeat display
                    nvm_cfg.flags.display_repeattimer_1_3_useonce = false;
                    nvm_cfg.flags.display_repeattimer_2_0_useonce = false;
                    nvm_cfg.flags.display_repeattimer_leaving_lasttime = false;

                    nvm_cfg.flags.chosen_timer++;
                } else {
                    nvm_cfg.flags.timer_flag = 0;
                    nvm_cfg.flags.chosen_timer = 0;     // could be unnecessary but for good measure
                    // calculate each timers set time in ms to cfg
                    for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
                        cfg.timers[i].set_time_in_ms = cfg.timers[i].set_day * 86400000 +
                                                    cfg.timers[i].set_hour * 3600000 +
                                                    cfg.timers[i].set_min * 60000 +
                                                    cfg.timers[i].set_sec * 1000 +
                                                    cfg.timers[i].set_ms;
                    }

                    // timer menu closed so write all cfg (timer) stuff to nvs
                    write_cfg_to_NVS();
                }
            }
            else {
                ESP_LOGI(BUTTON_TAG, "Timer repeat interval is active, no timer change allowed");
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
                // first entry
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 1;
            } else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1) {
                // leaving without setting to 0 (still first entry)
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 2;

                // calculate repeat interval to ms (when closing menu)
                cfg.timers[nvm_cfg.flags.chosen_timer].interval_in_ms = cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour * 3600000 +
                                              cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min * 60000 +
                                              cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec * 1000 +
                                              cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms;

                // no need to write to nvs as we do this when closing the timer menu
            } else if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2) {
                // leaving without setting to 0 (still first entry)
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 3;
            } else {
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer = 0;
                
                if (nvm_cfg.flags.cli_comm_rep_all == true) {
                    // loop over all timers and set the interval_in_ms for each timer back to original values, also do this for the set_time_in_ms
                    for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
                        // calculate repeat interval to ms for each timer (stopping cli command to repeat all timers)
                        cfg.timers[i].interval_in_ms = cfg.timers[i].repeat_interval_hour * 3600000 +
                                                cfg.timers[i].repeat_interval_min * 60000 +
                                                cfg.timers[i].repeat_interval_sec * 1000 +
                                                cfg.timers[i].repeat_interval_ms;

                        // calculate set time to ms for each timer (stopping cli command to repeat all timers)
                        cfg.timers[i].set_time_in_ms = cfg.timers[i].set_day * 86400000 +
                                                    cfg.timers[i].set_hour * 3600000 +
                                                    cfg.timers[i].set_min * 60000 +
                                                    cfg.timers[i].set_sec * 1000 +
                                                    cfg.timers[i].set_ms;
                    }
                }
                else {
                    // calculate repeat interval to ms (when closing menu), only if we did not do a cli command before
                    cfg.timers[nvm_cfg.flags.chosen_timer].interval_in_ms = cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour * 3600000 +
                                                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min * 60000 +
                                                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec * 1000 +
                                                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms;
                }

                // no need to write to nvs as we do this when closing the timer menu
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
            // calculate ms time from start of week (rtc time)
            nvm_cfg.flags.current_time_ms = (uint64_t)nvm_cfg.rtc.day * 86400000 +
                                            (uint64_t)nvm_cfg.rtc.hour * 3600000 +
                                            (uint64_t)nvm_cfg.rtc.min * 60000 +
                                            (uint64_t)nvm_cfg.rtc.sec * 1000;

            // Set cfg time to RTC
            set_ds3232_time(nvm_cfg.rtc.day, nvm_cfg.rtc.hour, nvm_cfg.rtc.min, nvm_cfg.rtc.sec);
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
        if (nvm_cfg.rtc.hour >= 23) {
            ESP_LOGI(BUTTON_TAG, "Clock hour limit reached (24), resetting to 0");
            nvm_cfg.rtc.hour = 0;
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
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1 || cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 3) {
            // update timer repeat interval
            // did we reach end of hour cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour >= 24) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat hour limit reached (25), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour = 0;
            }
            // up hour by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising hour for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour++;
            }
        }
        // change timer set time
        else if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0) || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2)) {
            // did we reach end of hour cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_hour >= 24) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment day limit reached (25), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_hour = 0;
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
        if (nvm_cfg.rtc.min >= 59) {
            ESP_LOGI(BUTTON_TAG, "Clock min limit reached (60), resetting to 0");
            nvm_cfg.rtc.min = 0;
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
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1 || cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 3) {
            // update timer repeat interval
            // did we reach end of min cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min >= 59) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat min limit reached (60), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min = 0;
            }
            // up min by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising min for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min++;
            }
        }
        // change timer set time
        else if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0) || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2)) {
            // did we reach end of min cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_min >= 59) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment min limit reached (60), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_min = 0;
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
        if (nvm_cfg.rtc.sec >= 59) {
            ESP_LOGI(BUTTON_TAG, "Clock sec limit reached (60), resetting to 0");
            nvm_cfg.rtc.sec = 0;
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
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1 || cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 3) {
            // update timer repeat interval
            // did we reach end of sec cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec >= 59) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat sec limit reached (60), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec = 0;
            }
            // up sec by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising sec for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec++;
            }
        }
        // change timer set time
        else if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0 || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2))) {
            // did we reach end of sec cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_sec >= 59) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment sec limit reached (60), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_sec = 0;
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
        if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1 || cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 3) {
            // update timer repeat interval
            // did we reach end of ms cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms >= 999) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer repeat ms limit reached (1000), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms = 0;
            }
            // up ms by one
            else {
                ESP_LOGI(BUTTON_TAG, "Raising ms for timer repeat by one to %d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms + 1);
                cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms++;
            }
        }
        // change timer set time
        else if ((cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 0 || (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2))) {
            // did we reach end of ms cycle?
            if (cfg.timers[nvm_cfg.flags.chosen_timer].set_ms >= 999) {
                ESP_LOGI(BUTTON_TAG, "Chosen timer setmoment ms limit reached (1000)), resetting to 0");
                cfg.timers[nvm_cfg.flags.chosen_timer].set_ms = 0;
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

