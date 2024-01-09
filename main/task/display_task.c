// includes
#include "task.h"

#define DISPLAY_TAG "DISPLAY"

// declare all display functions in here
// print some display init logs
void init_Display() {
    // feedback
    ESP_LOGI(DISPLAY_TAG, "Init Display");
 
#if CONFIG_I2C_INTERFACE
    ESP_LOGI(DISPLAY_TAG, "INTERFACE is i2c");
    ESP_LOGI(DISPLAY_TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
    ESP_LOGI(DISPLAY_TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
    ESP_LOGI(DISPLAY_TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif
 
#if CONFIG_SSD1306_128x64
    ESP_LOGI(DISPLAY_TAG, "Panel is 128x64");
    ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
    ssd1306_clear_screen(&dev, false);
}

// main display function
void Display_ssd1306(void *pvParameters) {
    struct timeval tv;
    struct tm timeinfo;
    // enter clock menu
    if (nvm_cfg.flags.clock_flag == true) {
        Display_Clock();
    }
    // enter timer menu
    else if (nvm_cfg.flags.timer_flag == true){
        // Display_timer();
    }
    // enter main menu
    else {
        // get current (system) time
        gettimeofday(&tv, NULL);

        // convert to local time
        localtime_r(&tv.tv_sec, &timeinfo);

        // format to string
        char strftime_buf[64];

        // format time to string
        strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S %d-%m", &timeinfo);

        // get day of week
        int day_of_week = timeinfo.tm_wday;

        // convert Sunday-based indexing to Monday-based indexing
        day_of_week = (day_of_week + 6 ) % 7;

        // define needed vars here (needed up next)
        int char_width = 8;
        int overline_position = ((day_of_week) * (char_width*2)) + 8;

        // show days array on display (shown immidiately)
        ssd1306_display_text(&dev, 0, " S M T W T F S ", 16, false);
        
        // show line under chosen day of week
        for (int i = 0; i < char_width; ++i) {
            dev._page[1]._segs[overline_position+i] = 0x01;
        }

        // show line defined above here (line under day of week)
        ssd1306_show_buffer(&dev);

        // show counting time on home page
        ssd1306_display_text(&dev, 5, strftime_buf, 16, false);
    }
    // delete task
    vTaskDelete(NULL);
}

// function to update display for clock
void Display_Clock() {
    // define function variables
    char strftime_buf_Display[64];
    struct tm timeinfo_temp;
    struct timeval tv_temp;
    
    // run this code once to init basic stuff for clock (function can/will be called more than once, this if will only run once)
    if (nvm_cfg.flags.updateElapsedTimeTask_useonce == false) {
        // feedback
        ESP_LOGI(DISPLAY_TAG, "Set clock init stuff");

        // get current time
        gettimeofday(&tv_temp, NULL);

        // convert to local time
        localtime_r(&tv_temp.tv_sec, &timeinfo_temp);

        // format to string
        strftime(strftime_buf_Display, sizeof(strftime_buf_Display), "%H:%M:%S %d", &timeinfo_temp);

        // make sure this runs once
        nvm_cfg.flags.updateElapsedTimeTask_useonce = true;

        // clear screen
        ssd1306_clear_screen(&dev,false);
        
        // show days array on display (shown immidiately)
        ssd1306_display_text(&dev, 0, " S M T W T F S ", 16, false);
    }

    // shows what day of the week it is on the display
    ssd1306_invert_segments(&dev, 0, nvm_cfg.rtc.day*2, nvm_cfg.rtc.day*2);
    ssd1306_show_buffer(&dev);
    
    // set cfg clock time (set by buttons) when clock flag is high to display (hour:min:sec:day)
    sprintf(strftime_buf_Display, "%02d:%02d:%02d", nvm_cfg.rtc.hour, nvm_cfg.rtc.min, nvm_cfg.rtc.sec);

    // show context on display
    ssd1306_display_text(&dev, 1, "Clock instellen", 16, false);
    ssd1306_display_text(&dev, 3, strftime_buf_Display, 16, false);
}

// function to update display for timer
void Display_timer(){
    // define function variables
    char strftime_buf_Timer[64];
    // bool justonce_timer = false;
    // bool justonce_Herhaalmoment = false;

    //er moet een regel zijn die laat zien of er een herhaalmoment is ingeschakeld // misschien tijd laten zien
    // wanneer er op herhaalmoment wordt gedrukt moet er duidelijk een ander menu laten zien waar vervolgens ook een tijd wordt laten zien.
    // er wordt in het vervolg menu geen dagen laten zien.

    // are we repeating this specific chosen timer?
    if (cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 1 || cfg.timers[nvm_cfg.flags.chosen_timer].repeat_timer == 2) {
        // do this only once when we first enter this timer repeat flag high
        if (nvm_cfg.flags.display_repeattimer_useonce == false) {
            // clear screen (timer screen)
            ssd1306_clear_screen(&dev,false);

            // set nvm_cfg.flags.display_repeattimer_useonce to true so we do not enter here again
            nvm_cfg.flags.display_repeattimer_useonce = true;
        }
        
        // show context on display
        ssd1306_display_text(&dev, 1, "Herhaaltijd", 13, false);
        // show repeat time on display
        sprintf(strftime_buf_Timer, "%02d:%02d:%02d:%04d", cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour, cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min, cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec, cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms);
        ssd1306_display_text(&dev, 3, strftime_buf_Timer, 16, false);
    }
    // not repeat menu so timer menu
    else {
        // do this only once when we first enter this timer flag high
        if (nvm_cfg.flags.display_timer_useonce == false) {
            // clear screen (timer screen)
            ssd1306_clear_screen(&dev,false);

            // show context on display
            ssd1306_display_text(&dev, 1, "Timer instellen", 16, false);
            // show days array on display (shown immidiately)
            ssd1306_display_text(&dev, 2, " S M T W T F S ", 16, false);
         }

        // show what timer is selected
        ssd1306_display_text(&dev, 0, nvm_cfg.flags.chosen_timer, 16, false);

        // show the day that is selected for timer moment
        ssd1306_invert_segments(&dev, 2, cfg.timers[nvm_cfg.flags.chosen_timer].set_day*2, cfg.timers[nvm_cfg.flags.chosen_timer].set_day*2);   // "TEST" look into that sometimes the day will flikker on other days with constant interval, look into

        // show on display if timer is on or off
        if (cfg.timers[nvm_cfg.flags.chosen_timer].timer_active == 1) {
            // empty specific line
            // ssd1306_clear_line(&dev, 3, false);
            // fill line with context
            ssd1306_display_text(&dev, 3, "Timer is on", 16, false);
        }
        else if(cfg.timers[nvm_cfg.flags.chosen_timer].timer_active == 0) {
            // empty specific line
            // ssd1306_clear_line(&dev, 3, false);
            // fill line with context
            ssd1306_display_text(&dev, 3, "Timer is off", 16, false);
        }
        
        // show timer moment on display
        sprintf(strftime_buf_Timer, "%02d:%02d:%02d:%04d", cfg.timers[nvm_cfg.flags.chosen_timer].set_hour, cfg.timers[nvm_cfg.flags.chosen_timer].set_min, cfg.timers[nvm_cfg.flags.chosen_timer].set_sec, cfg.timers[nvm_cfg.flags.chosen_timer].set_ms);
        ssd1306_display_text(&dev, 5, strftime_buf_Timer, 16, false);

        // show output setpoint high or low on timer moment
        ssd1306_display_text(&dev, 6, cfg.timers[nvm_cfg.flags.chosen_timer].set_value, 16, false);
    }
}