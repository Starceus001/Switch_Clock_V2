#ifndef MAIN_H
#define MAIN_H

// includes (needed inside main.c or main.h)
#include <stdint.h>
#include <stdio.h>

struct cfg_t;
struct nvm_cfg_t;
 
// Global defines:
// Digital input pins
#define DAG_KNOP GPIO_NUM_25
#define TIMER_KNOP GPIO_NUM_12
#define TIMER_ACTIEF_KNOP GPIO_NUM_4
#define SCHAKELUITGANG_AANUIT_KNOP GPIO_NUM_16
#define HERHAALSCHAKELMOMENT_KNOP GPIO_NUM_17
#define NUM_DIGITAL_BUTTONS 5

// Analog input pins
#define CLOCK_KNOP ADC1_CHANNEL_0
#define UUR_KNOP ADC1_CHANNEL_3
#define MINUUT_KNOP ADC1_CHANNEL_6
#define SECONDEN_KNOP ADC1_CHANNEL_7
#define MSCENONDE_KNOP ADC2_CHANNEL_3
#define NUM_ANALOG_BUTTONS 5

// Digital output pins
#define OUTPUT_1 GPIO_NUM_26
#define OUTPUT_2 GPIO_NUM_14
#define OUTPUT_3 GPIO_NUM_13
#define OUTPUT_4 GPIO_NUM_2

#define ANALOG_THRESHOLD            200         // 3V3 gives 255 value, 0V gives 0 value, above 200 is considered HIGH
#define DS3232_ADDRESS              0x68        // 7-bit I2C address
#define MAX_TIMER_COUNT             3
#define ANALOG_DEBOUNCE_DELAY_MS    200
#define DIGITAL_DEBOUNCE_DELAY      500

// structs (should be called in each file where it is needed)
typedef struct {
    uint8_t clock_flag : 1;
    uint8_t timer_flag : 1;

    uint8_t chosen_timer;
} flags_t;

typedef struct {
    uint16_t rtc_read_time;
    uint8_t day;                        // max 7 days
    uint8_t hour;                       // max 24 hours
    uint8_t min;                        // max 60 min
    uint8_t sec;                        // max 60 sec
} rtc_t;

typedef struct {
    // uint16_t time;                   // look into how to bring all time data into one var? or check all individual (day, hour, min, sec, ms)
    uint8_t set_day;                    // max 7 days
    uint8_t set_hour;                   // max 24 hours
    uint8_t set_min;                    // max 60 min
    uint8_t set_sec;                    // max 60 sec
    uint16_t set_ms;                    // max 999 ms
    uint8_t timer_active;
    uint8_t set_value;

    uint8_t repeat_timer;
    uint8_t repeat_interval_hour;       // max 24 hours
    uint8_t repeat_interval_min;        // max 60 min
    uint8_t repeat_interval_sec;        // max 60 sec
    uint16_t repeat_interval_ms;        // max 999 ms
} timers_t;

// this struct will be remembered in EEPROM when power turns off
typedef struct {
    // timers data
    timers_t timers[MAX_TIMER_COUNT];
} cfg_t;

// this struct will empty when power turns off
typedef struct {
    // flags
    flags_t flags;

    // rtc data
    rtc_t rtc;
} nvm_cfg_t;

// main function definitions
extern void startup_logs();

#endif // MAIN_H