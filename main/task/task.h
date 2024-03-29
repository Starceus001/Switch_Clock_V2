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
#include <sys/time.h>
#include "esp_sntp.h"
#include <esp_task_wdt.h>
#include "driver/uart.h"
#include <ctype.h>
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

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
extern void uart_init();

extern void read_cli_constant();

extern void handle_command(char* command);

extern void cli_command_set_time(char* command);

extern void cli_command_timer_set(char* command);

extern void cli_command_timers_all_rep(char* command);

extern void cli_command_cfg_print(char* command);

extern void cli_command_help(char* command);

extern void trim_whitespace(char* str);

// --<< display_task.c >>--
extern void init_Display();

extern void Display_ssd1306(void *pvParameters);

extern void display_refresh_visual();

extern void Display_clear_screen();

extern void Display_Clock();

extern void Display_timer();

extern void update_day_indicator(uint8_t display_line, uint8_t display_menu, uint8_t timer_choice);

// --<< ms_timer_task.c >>--
// extern void updateElapsedTimeTask(void *pvParameters);

extern void init_ms_outputs_intr();

extern void set_outputs_intr_callback();

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