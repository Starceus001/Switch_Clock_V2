// includes
#include "task.h"

// declare all button functions in here
// declare all esp32 functions in here
void init_gpio() {
    // Configure digital input pins as inputs
        gpio_config_t digital_input_conf = {
            .pin_bit_mask = (1ULL << DAG_KNOP) | (1ULL << TIMER_KNOP) | (1ULL << TIMER_ACTIEF_KNOP) | (1ULL << SCHAKELUITGANG_AANUIT_KNOP) | (1ULL << HERHAALSCHAKELMOMENT_KNOP),
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&digital_input_conf);
 
        // Configure digital output pins as outputs
        gpio_config_t digital_output_conf = {
            .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3)| (1ULL << OUTPUT_4),
            .mode = GPIO_MODE_INPUT_OUTPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&digital_output_conf);
 
        // Configure analog input pins
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(CLOCK_KNOP, ADC_ATTEN_DB_0);
        adc1_config_channel_atten(UUR_KNOP, ADC_ATTEN_DB_0);
        adc1_config_channel_atten(MINUUT_KNOP, ADC_ATTEN_DB_0);
        adc1_config_channel_atten(SECONDEN_KNOP, ADC_ATTEN_DB_0);
        adc1_config_channel_atten(MSCENONDE_KNOP, ADC_ATTEN_DB_0);
}

// check all inputs?
void check_analog_inputs_task() {
    while (1) {
        // Read analog input values
        // int analog_value_1 = adc1_get_raw(MSCENONDE_KNOP);
        // int analog_value_2 = adc1_get_raw(UUR_KNOP);
        // int analog_value_3 = adc1_get_raw(MINUUT_KNOP);
        // int analog_value_4 = adc1_get_raw(SECONDEN_KNOP);

        // nvm_cfg.buttons.ms = adc1_get_raw(MSCENONDE_KNOP);
        // nvm_cfg.buttons.hour = adc1_get_raw(UUR_KNOP);
        // nvm_cfg.buttons.min = adc1_get_raw(MINUUT_KNOP);
        // nvm_cfg.buttons.sec = adc1_get_raw(SECONDEN_KNOP);
// "TEST" change to nvm cfg vars!
        // Check analog values and set digital output pins accordingly
        // if (analog_value_1 > ANALOG_THRESHOLD) {
        //     if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
        //         gpio_set_level(OUTPUT_1, !gpio_get_level(OUTPUT_1));
        //         xSemaphoreGive(xSemaphore);
        //     }
        // }
 
        // if (analog_value_2 > ANALOG_THRESHOLD) {
        //     if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
        //         gpio_set_level(OUTPUT_2, !gpio_get_level(OUTPUT_2));
        //         xSemaphoreGive(xSemaphore);
        //     }
        // }
 
        // if (analog_value_3 > ANALOG_THRESHOLD) {
        //     if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
        //         gpio_set_level(OUTPUT_3, !gpio_get_level(OUTPUT_3));
        //         xSemaphoreGive(xSemaphore);
        //     }
        // }
 
        // if (analog_value_4 > ANALOG_THRESHOLD) {
        //     if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
        //         gpio_set_level(OUTPUT_4, !gpio_get_level(OUTPUT_4));
        //         xSemaphoreGive(xSemaphore);
        //     }
        // }
 
        // vTaskDelay(pdMS_TO_TICKS(100));  // Adjust the delay based on your application requirements
    }
}