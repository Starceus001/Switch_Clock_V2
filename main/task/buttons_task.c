// includes
#include "task.h"

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

void handle_button_press(gpio_num_t buttonPin) {
    if (xSemaphoreTake(buttonSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
        // Print the button pin number
        printf("Button on pin %d pressed\n", buttonPin);

        // You can perform additional actions here based on the button press
        // "TEST" use this function when all button reading works to do things with the flags after a flag set (like updating a clock var with the time buttons or updating the display if chosen not to do that in the main loop)
        // switch (buttonPin) {
        //     case DAG_KNOP:
        //         // Handle DAG_KNOP button press
        //         break;
        //     case TIMER_KNOP:
        //         // Handle TIMER_KNOP button press
        //         break;
        //     // Add cases for other buttons as needed
        // }

        xSemaphoreGive(buttonSemaphore);  // Release the semaphore
    }
}

// declare all button functions in here
void init_gpio() {
    // Configure digital output pins as outputs
    gpio_config_t digital_output_conf = {
        .pin_bit_mask = (1ULL << OUTPUT_1) | (1ULL << OUTPUT_2) | (1ULL << OUTPUT_3) | (1ULL << OUTPUT_4),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&digital_output_conf);

    // Configure digital and analog input pins as inputs and set up interrupts
    gpio_config_t input_conf = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    for (int i = 0; i < NUM_DIGITAL_BUTTONS + NUM_ANALOG_BUTTONS; i++) {
        input_conf.pin_bit_mask = (1ULL << all_buttons[i]);
        gpio_config(&input_conf);
        gpio_install_isr_service(0);
        gpio_isr_handler_add(all_buttons[i], button_isr_handler, (void*)all_buttons[i]);
    }

// Analog buttons may require additional configuration (e.g., ADC setup) depending on your requirements
    // Configure analog input pins
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(CLOCK_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(UUR_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MINUUT_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(SECONDEN_KNOP, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(MSCENONDE_KNOP, ADC_ATTEN_DB_0);

    for (int i = 0; i < NUM_ANALOG_BUTTONS; i++) {
        // Additional configuration for analog buttons, if needed
        // For example, configuring ADC channels and attenuations
        // adc1_config_channel_atten(analog_buttons[i], ADC_ATTEN_DB_0);

        // Add interrupt setup for analog buttons, if needed
        // gpio_config(&input_conf);
        // gpio_install_isr_service(0);
        // gpio_isr_handler_add(analog_buttons[i], button_isr_handler, (void*)analog_buttons[i]);
    }

    // Create a binary semaphore for synchronizing access to shared resources
    buttonSemaphore = xSemaphoreCreateBinary();
}

void button_isr_handler(void* arg) {
    gpio_num_t buttonPin = (gpio_num_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Set the flag corresponding to the pressed button
    // set_button_flag(buttonPin);         // "TEST" implement this function with a switch case to set the correct flag inside the nvm_cfg!! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    // Notify the task that a button was pressed
    xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);

    // If using FreeRTOS, yield to the task if a higher-priority task was woken
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}