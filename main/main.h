#ifndef MAIN_H
#define MAIN_H

// includes (needed inside main.c or main.h)
#include <stdint.h>
#include <stdio.h>

struct cfg_t;
struct nvm_cfg_t;

// Digital input pins
#define DAG_KNOP GPIO_NUM_25
#define TIMER_KNOP GPIO_NUM_12
#define TIMER_ACTIEF_KNOP GPIO_NUM_4
#define SCHAKELUITGANG_AANUIT_KNOP GPIO_NUM_16
#define HERHAALSCHAKELMOMENT_KNOP GPIO_NUM_17

// Digital output pins
#define OUTPUT_1 GPIO_NUM_26
#define OUTPUT_2 GPIO_NUM_14
#define OUTPUT_3 GPIO_NUM_13
#define OUTPUT_4 GPIO_NUM_2

// Analog input pins
#define CLOCK_KNOP ADC1_CHANNEL_0
#define UUR_KNOP ADC1_CHANNEL_3
#define MINUUT_KNOP ADC1_CHANNEL_6
#define SECONDEN_KNOP ADC1_CHANNEL_7
#define MSCENONDE_KNOP ADC2_CHANNEL_3

#define ANALOG_THRESHOLD    1500
#define tag "SSD1306"
#define DS3232_ADDRESS      0x68  // 7-bit I2C address

#define MAX_TIMER_COUNT     4

// structs (should be called in each file where it is needed)
typedef struct {
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t ms;
    uint8_t clock;
    uint8_t timer;
    uint8_t timer_active;
    uint8_t output_onoff;
    uint8_t timer_repeat;
} buttons_t;

typedef struct {
    uint8_t clock_flag : 1;
    uint8_t timer_flag : 1;  
} flags_t;

typedef struct {
    uint16_t rtc_time;
} rtc_t;

typedef struct {
    uint16_t time;
    uint8_t set_value;
    uint8_t repeat;
} timers_t;

// this struct will be remembered when power turns off
typedef struct {
    // timers data
    timers_t timers[MAX_TIMER_COUNT];
} cfg_t;

// this struct will empty when power turns off
typedef struct {
    // reading data of buttons
    buttons_t buttons;
    
    // flags
    flags_t flags;

    // rtc data
    rtc_t rtc;
} nvm_cfg_t;

// main function definitions
// ...

#endif // MAIN_H