#ifndef TASK_H
#define TASK_H

// includes (needed inside any of the tasks.c or task.h)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include <freertos/semphr.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include <time.h>
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <esp_err.h>
#include "nvs_flash.h"
#include "nvs.h"
// #include "ssd1306.h"
// #include "font8x8_basic.h"
// #include "esp_sntp.h"
// #include <sys/time.h>
// #include <esp_task_wdt.h>
// #include "driver/uart.h"
// #include <ctype.h>

#include "main.h"

// Declare the structs as extern (so it can be called from any task.c file)
extern cfg_t cfg;
extern nvm_cfg_t nvm_cfg;

// task function definitions
// --<< buttons_task.c >>--
extern void init_gpio();

extern void button_handle_task(void* arg);

extern void handle_button_press(gpio_num_t buttonPin);

extern void digital_button_isr_handler(void* arg);

extern void analog_button_check_task(void* arg);

extern void DAG_KNOP_button_pressed();

extern void TIMER_KNOP_button_pressed();

extern void TIMER_ACTIEF_KNOP_button_pressed();

extern void SCHAKELUITGANG_AANUIT_KNOP_button_pressed();

extern void HERHAALSCHAKELMOMENT_KNOP_button_pressed();

extern void CLOCK_KNOP_button_pressed();

extern void UUR_KNOP_button_pressed();

extern void MINUUT_KNOP_button_pressed();

extern void SECONDEN_KNOP_button_pressed();

extern void MSCENONDE_KNOP_button_pressed();

// --<< cfg_task.c >>--
extern void init_cfg();

extern void preset_nvmcfg();

extern void cfg_print();

extern void print_cfg();

extern void print_nvm_cfg();

// --<< cli_task.c >>--
extern void read_cli_constant();

extern void handle_command(const char* command);

extern void cli_command_set_time(char* command);

extern void cli_command_timer_rep(char* command);

extern void cli_command_cfg_print(char* command);

extern void cli_command_help(char* command);

// --<< display_task.c >>--
extern void init_Display();

extern void Display_ssd1306(void *pvParameters);

extern void Display_Clock();

extern void Display_timer();

// --<< ms_timer_task.c >>--
extern void updateElapsedTimeTask(void *pvParameters);

extern void check_timers_time_to_system_time(uint16_t counting_ms);

extern void set_timer_output(uint8_t timer_number);

extern void timer_callback(void* arg);

extern void timer_start_periodic(uint32_t mseconds, int timer_index, gpio_num_t output_pin);

// --<< nvs_task.c >>--
extern void init_NVS();

extern void write_cfg_to_NVS();

extern void read_cfg_from_NVS();

// --<< rtc_task.c >>--
extern void set_ds3232_time(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);

extern void read_ds3232_task(void *pvParameters);

extern void set_system_time_from_ds3232(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint16_t year);

extern uint8_t dec_to_bcd(uint8_t val);

extern void read_system_time_to_cfg();

#endif // TASK_H