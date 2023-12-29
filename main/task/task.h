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
// #include "path/to/ssd1306.h"     // to download
#include "driver/i2c.h"
#include <time.h>
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <esp_err.h>

#include <main/main.h>

// Declare the structs as extern (so it can be called from any task.c file)
extern cfg_t cfg;
extern nvm_cfg_t nvm_cfg;

SemaphoreHandle_t cfgMutex;  // Mutex to synchronize access to cfg

// task function definitions
// --<< buttons_task.c >>--
extern void init_gpio();

extern void handle_button_press(gpio_num_t buttonPin);

extern void button_isr_handler(void* arg);

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

// --<< cores_task.c >>--
// ...

// --<< display_task.c >>--
extern void init_display();

// --<< ms_timer_task.c >>--
extern void timer_start();

// --<< rtc_task.c >>--
extern uint8_t dec_to_bcd(uint8_t val);

extern void read_ds3232_task();

extern void set_ds3232_time(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);

extern void set_ds3232_to_system_time();

extern void synchronize_system_time_with_ds3232();

// --<< switch_timer_task.c >>--
extern void ms_timer_start(void *pvParameters);

extern void timer_callback(void* arg);

#endif // TASK_H