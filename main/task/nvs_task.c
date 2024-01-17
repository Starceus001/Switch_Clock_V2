// includes
#include "task.h"

#define NVS_TAG "NVS"

// declare all NVS functions in here
void init_NVS() {
    // feedback
    ESP_LOGI(NVS_TAG, "Initializing NVS");

    // init NVS and do error checking
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    // log the error
    ESP_ERROR_CHECK(err);

    // feedback
    ESP_LOGI(NVS_TAG, "Initializing NVS done");
}

void write_cfg_to_NVS() {
    // feedback
    ESP_LOGI(NVS_TAG, "Write cfg to NVS");

    // open nvs
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("cfg", NVS_READWRITE, &nvs_handle));

    // read all timer data from NVS (iterate over all timers)
    for (int i = 0; i <= MAX_TIMER_COUNT; i++) {
        // key variable to hold the key name for the NVS data
        char key[16];

        // write the data from the NVS based on the created key for each variable, data will be read from our struct
        // set_day
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_day: %d", i, cfg.timers[i].set_day);
        sprintf(key, "tmr%d_set_day", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_day));
        // set_hour
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_hour: %d", i, cfg.timers[i].set_hour);
        sprintf(key, "tmr%d_set_hour", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_hour));
        // set_min
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_min: %d", i, cfg.timers[i].set_min);
        sprintf(key, "tmr%d_set_min", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_min));
        // set_sec
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_sec: %d", i, cfg.timers[i].set_sec);
        sprintf(key, "tmr%d_set_sec", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_sec));
        // set_ms
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_ms: %d", i, cfg.timers[i].set_ms);
        sprintf(key, "tmr%d_set_ms", i);
        ESP_ERROR_CHECK(nvs_set_u16(nvs_handle, key, cfg.timers[i].set_ms));
        // timer_active
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] timer_active: %d", i, cfg.timers[i].timer_active);
        sprintf(key, "tmr%d_active", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].timer_active));
        // set_value
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_value: %d", i, cfg.timers[i].set_value);
        sprintf(key, "tmr%d_set_value", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_value));
        // repeat_timer
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_timer: %d", i, cfg.timers[i].repeat_timer);
        sprintf(key, "tmr%d_rpt", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_timer));
        // repeat_interval_hour
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_hour: %d", i, cfg.timers[i].repeat_interval_hour);
        sprintf(key, "tmr%d_rpt_hour", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_hour));
        // repeat_interval_min
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_min: %d", i, cfg.timers[i].repeat_interval_min);
        sprintf(key, "tmr%d_rpt_min", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_min));
        // repeat_interval_sec
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_sec: %d", i, cfg.timers[i].repeat_interval_sec);
        sprintf(key, "tmr%d_rpt_sec", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_sec));
        // repeat_interval_ms
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_ms: %d", i, cfg.timers[i].repeat_interval_ms);
        sprintf(key, "tmr%d_rpt_ms", i);
        ESP_ERROR_CHECK(nvs_set_u16(nvs_handle, key, cfg.timers[i].repeat_interval_ms));
    }

    // Commit the changes to NVS
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));

    // close nvs after use
    nvs_close(nvs_handle);

    // feedback
    ESP_LOGI(NVS_TAG, "Write cfg to NVS done");
}

void read_cfg_from_NVS() {
    // feedback
    ESP_LOGI(NVS_TAG, "Read cfg from NVS");

    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("cfg", NVS_READONLY, &nvs_handle));

    // read individual components of the struct
    for (int i = 0; i <= MAX_TIMER_COUNT; i++) {
        // key variable to hold the key name for the NVS data
        char key[16];

        // read the data from the NVS based on the created key for each variable, data will be read into our struct
        // set_day
        sprintf(key, "tmr%d_set_day", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_day));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_day: %d", i, cfg.timers[i].set_day);
        // set_hour
        sprintf(key, "tmr%d_set_hour", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_hour));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_hour: %d", i, cfg.timers[i].set_hour);
        // set_min
        sprintf(key, "tmr%d_set_min", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_min));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_min: %d", i, cfg.timers[i].set_min);
        // set_sec
        sprintf(key, "tmr%d_set_sec", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_sec));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_sec: %d", i, cfg.timers[i].set_sec);
        // set_ms
        sprintf(key, "tmr%d_set_ms", i);
        ESP_ERROR_CHECK(nvs_get_u16(nvs_handle, key, &cfg.timers[i].set_ms));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_ms: %d", i, cfg.timers[i].set_ms);
        // timer_active
        sprintf(key, "tmr%d_active", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].timer_active));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] timer_active: %d", i, cfg.timers[i].timer_active);
        // set_value
        sprintf(key, "tmr%d_set_value", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_value));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_value: %d", i, cfg.timers[i].set_value);
        // repeat_timer
        sprintf(key, "tmr%d_rpt", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_timer));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_timer: %d", i, cfg.timers[i].repeat_timer);
        // repeat_interval_hour
        sprintf(key, "tmr%d_rpt_hour", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_hour));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_hour: %d", i, cfg.timers[i].repeat_interval_hour);
        // repeat_interval_min
        sprintf(key, "tmr%d_rpt_min", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_min));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_min: %d", i, cfg.timers[i].repeat_interval_min);
        // repeat_interval_sec
        sprintf(key, "tmr%d_rpt_sec", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_sec));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_sec: %d", i, cfg.timers[i].repeat_interval_sec);
        // repeat_interval_ms
        sprintf(key, "tmr%d_rpt_ms", i);
        ESP_ERROR_CHECK(nvs_get_u16(nvs_handle, key, &cfg.timers[i].repeat_interval_ms));
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_ms: %d", i, cfg.timers[i].repeat_interval_ms);

        // calculate set_time_in_ms for each timer
        cfg.timers[i].set_time_in_ms = cfg.timers[i].set_day * 86400000 +
                                                    cfg.timers[i].set_hour * 3600000 +
                                                    cfg.timers[i].set_min * 60000 +
                                                    cfg.timers[i].set_sec * 1000 +
                                                    cfg.timers[i].set_ms;

        // calculate interval_in_ms for each timer
        cfg.timers[i].interval_in_ms = cfg.timers[i].repeat_interval_hour * 3600000 +
                                              cfg.timers[i].repeat_interval_min * 60000 +
                                              cfg.timers[i].repeat_interval_sec * 1000 +
                                              cfg.timers[i].repeat_interval_ms;
    }
    
    // close nvs after use
    nvs_close(nvs_handle);

    // feedback
    ESP_LOGI(NVS_TAG, "Read cfg from NVS done");
}