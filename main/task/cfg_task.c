// includes
#include "task.h"

#define CFG_TAG "CFG"

// declare all cfg functions in here
void preset_nvmcfg() {
    // feedback
    ESP_LOGI(CFG_TAG, "Preset nvm_cfg");

    // init flags nvm preset values
    nvm_cfg.flags.clock_flag = 0;
    nvm_cfg.flags.timer_flag = 0;
    nvm_cfg.flags.chosen_timer = 0;
    // init display nvm preset values
    nvm_cfg.flags.updateElapsedTimeTask_useonce = false;
    nvm_cfg.flags.timerset_display = false;
    nvm_cfg.flags.display_timer_useonce = false;
    nvm_cfg.flags.display_repeattimer_useonce = false;

    // init rtc nvm preset values
    nvm_cfg.rtc.day = 0;
    nvm_cfg.rtc.hour = 0;
    nvm_cfg.rtc.min = 0;
    nvm_cfg.rtc.sec = 0;

    // feedback
    ESP_LOGI(CFG_TAG, "Preset nvm_cfg done");
}

void init_cfg() {
    // feedback
    ESP_LOGI(CFG_TAG, "Initialize cfg");

    // fill cfg and nvm cfg with preset values
    preset_nvmcfg();
    
    // init nvs
    init_NVS();

    // read the cfg_t struct from NVS
    read_cfg_from_NVS();

    // feedback
    ESP_LOGI(CFG_TAG, "Initialize cfg done");
}

void cfg_print() {
    // print cfg_t
    print_cfg();

    // print nvm_cfg_t
    print_nvm_cfg();
}

void print_cfg() {
    // feedback
    ESP_LOGI(CFG_TAG, "Printing cfg");

    ESP_LOGI(CFG_TAG, "------------------------------------------------------");
    ESP_LOGI(CFG_TAG, "[TIMER] Timer count: %d", MAX_TIMER_COUNT+1);
    // loop over all timers
    for (int i = 0; i <= MAX_TIMER_COUNT; i++) {
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_day: %d", i, cfg.timers[i].set_day);
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_hour: %d", i, cfg.timers[i].set_hour);
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_min: %d", i, cfg.timers[i].set_min);
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_sec: %d", i, cfg.timers[i].set_sec);
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_ms: %d", i, cfg.timers[i].set_ms);
        ESP_LOGI(CFG_TAG, "[TIMER %d] timer_active: %d", i, cfg.timers[i].timer_active);
        ESP_LOGI(CFG_TAG, "[TIMER %d] set_value: %d", i, cfg.timers[i].set_value);
        ESP_LOGI(CFG_TAG, "[TIMER %d] repeat_timer: %d", i, cfg.timers[i].repeat_timer);
        ESP_LOGI(CFG_TAG, "[TIMER %d] repeat_interval_hour: %d", i, cfg.timers[i].repeat_interval_hour);
        ESP_LOGI(CFG_TAG, "[TIMER %d] repeat_interval_min: %d", i, cfg.timers[i].repeat_interval_min);
        ESP_LOGI(CFG_TAG, "[TIMER %d] repeat_interval_sec: %d", i, cfg.timers[i].repeat_interval_sec);
        ESP_LOGI(CFG_TAG, "[TIMER %d] repeat_interval_ms: %d", i, cfg.timers[i].repeat_interval_ms);
    }
    ESP_LOGI(CFG_TAG, "------------------------------------------------------");

    // feedback
    ESP_LOGI(CFG_TAG, "Printing cfg done");
}

void print_nvm_cfg() {
    // feedback
    ESP_LOGI(CFG_TAG, "Printing nvm_cfg");

    // flags
    ESP_LOGI(CFG_TAG, "------------------------------------------------------");
    ESP_LOGI(CFG_TAG, "[FLAGS] clock_flag: %d", nvm_cfg.flags.clock_flag);
    ESP_LOGI(CFG_TAG, "[FLAGS] timer_flag: %d", nvm_cfg.flags.timer_flag);
    ESP_LOGI(CFG_TAG, "[FLAGS] chosen_timer: %d", nvm_cfg.flags.chosen_timer);

    // rtc
    ESP_LOGI(CFG_TAG, "[RTC] day: %d", nvm_cfg.rtc.day);
    ESP_LOGI(CFG_TAG, "[RTC] hour: %d", nvm_cfg.rtc.hour);
    ESP_LOGI(CFG_TAG, "[RTC] min: %d", nvm_cfg.rtc.min);
    ESP_LOGI(CFG_TAG, "[RTC] sec: %d", nvm_cfg.rtc.sec);
    ESP_LOGI(CFG_TAG, "------------------------------------------------------");

    // feedback
    ESP_LOGI(CFG_TAG, "Printing nvm_cfg done");
}
