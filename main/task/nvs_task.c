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
}

void write_cfg_to_NVS() {
    // feedback
    ESP_LOGI(NVS_TAG, "Write cfg to NVS");

    // open nvs
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("cfg", NVS_READWRITE, &nvs_handle));

    // read all timer data from NVS (iterate over all timers)
    for (int i = 0; i < MAX_TIMER_COUNT; i++) {
        // key variable to hold the key name for the NVS data
        char key[20];

        // write the data from the NVS based on the created key for each variable, data will be read from our struct
        // set_day
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_day: %d", i, cfg.timers[i].set_day);
        sprintf(key, "timer_%d_set_day", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_day));
        // set_hour
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_hour: %d", i, cfg.timers[i].set_hour);
        sprintf(key, "timer_%d_set_hour", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_hour));
        // set_min
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_min: %d", i, cfg.timers[i].set_min);
        sprintf(key, "timer_%d_set_min", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_min));
        // set_sec
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_sec: %d", i, cfg.timers[i].set_sec);
        sprintf(key, "timer_%d_set_sec", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_sec));
        // set_ms
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_ms: %d", i, cfg.timers[i].set_ms);
        sprintf(key, "timer_%d_set_ms", i);
        ESP_ERROR_CHECK(nvs_set_u16(nvs_handle, key, cfg.timers[i].set_ms));
        // timer_active
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] timer_active: %d", i, cfg.timers[i].timer_active);
        sprintf(key, "timer_%d_active", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].timer_active));
        // set_value
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] set_value: %d", i, cfg.timers[i].set_value);
        sprintf(key, "timer_%d_set_value", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].set_value));
        // repeat_timer
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_timer: %d", i, cfg.timers[i].repeat_timer);
        sprintf(key, "timer_%d_repeat", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_timer));
        // repeat_interval_hour
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_hour: %d", i, cfg.timers[i].repeat_interval_hour);
        sprintf(key, "timer_%d_repeat_hour", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_hour));
        // repeat_interval_min
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_min: %d", i, cfg.timers[i].repeat_interval_min);
        sprintf(key, "timer_%d_repeat_min", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_min));
        // repeat_interval_sec
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_sec: %d", i, cfg.timers[i].repeat_interval_sec);
        sprintf(key, "timer_%d_repeat_sec", i);
        ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, key, cfg.timers[i].repeat_interval_sec));
        // repeat_interval_ms
        ESP_LOGI(NVS_TAG, "Writing NVS - [timer %d] repeat_interval_ms: %d", i, cfg.timers[i].repeat_interval_ms);
        sprintf(key, "timer_%d_repeat_ms", i);
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
    for (int i = 0; i < MAX_TIMER_COUNT; i++) {
        // key variable to hold the key name for the NVS data
        char key[20];

        // read the data from the NVS based on the created key for each variable, data will be read into our struct
        // set_day
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_day: %d", i, cfg.timers[i].set_day);
        sprintf(key, "timer_%d_set_day", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_day));
        // set_hour
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_hour: %d", i, cfg.timers[i].set_hour);
        sprintf(key, "timer_%d_set_hour", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_hour));
        // set_min
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_min: %d", i, cfg.timers[i].set_min);
        sprintf(key, "timer_%d_set_min", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_min));
        // set_sec
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_sec: %d", i, cfg.timers[i].set_sec);
        sprintf(key, "timer_%d_set_sec", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_sec));
        // set_ms
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_ms: %d", i, cfg.timers[i].set_ms);
        sprintf(key, "timer_%d_set_ms", i);
        ESP_ERROR_CHECK(nvs_get_u16(nvs_handle, key, &cfg.timers[i].set_ms));
        // timer_active
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] timer_active: %d", i, cfg.timers[i].timer_active);
        sprintf(key, "timer_%d_active", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].timer_active));
        // set_value
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] set_value: %d", i, cfg.timers[i].set_value);
        sprintf(key, "timer_%d_set_value", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].set_value));
        // repeat_timer
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_timer: %d", i, cfg.timers[i].repeat_timer);
        sprintf(key, "timer_%d_repeat", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_timer));
        // repeat_interval_hour
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_hour: %d", i, cfg.timers[i].repeat_interval_hour);
        sprintf(key, "timer_%d_repeat_hour", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_hour));
        // repeat_interval_min
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_min: %d", i, cfg.timers[i].repeat_interval_min);
        sprintf(key, "timer_%d_repeat_min", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_min));
        // repeat_interval_sec
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_sec: %d", i, cfg.timers[i].repeat_interval_sec);
        sprintf(key, "timer_%d_repeat_sec", i);
        ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, key, &cfg.timers[i].repeat_interval_sec));
        // repeat_interval_ms
        ESP_LOGI(NVS_TAG, "Reading NVS - [timer %d] repeat_interval_ms: %d", i, cfg.timers[i].repeat_interval_ms);
        sprintf(key, "timer_%d_repeat_ms", i);
        ESP_ERROR_CHECK(nvs_get_u16(nvs_handle, key, &cfg.timers[i].repeat_interval_ms));
    }
    
    // close nvs after use
    nvs_close(nvs_handle);

    // feedback
    ESP_LOGI(NVS_TAG, "Read NVS to cfg done");
}





// initialize EEPROM
// void init_EEPROM() {
//     // feedback
//     ESP_LOGI(EEPROM_TAG, "Initializing EEPROM");

//     // init EEPROM
//     EEPROM.begin(sizeof(cfg_t));

//     // read the cfg_t struct from EEPROM
//     read_config_from_EEPROM(cfg_t &config);
// }

// // read the cfg_t struct from EEPROM
// void read_config_from_EEPROM(cfg_t &config) {
//     // feedback
//     ESP_LOGI(EEPROM_TAG, "Reading cfg from EEPROM");
    
//     // get the cfg_t struct from EEPROM
//     EEPROM.get(EEPROM_ADDR_CFG, config);
// }

// // write the cfg_t struct to EEPROM
// void write_config_to_EEPROM(const cfg_t &config) {
//     // feedback
//     ESP_LOGI(EEPROM_TAG, "Writing cfg to EEPROM");

//     // prepare the cfg_t struct to be written to EEPROM
//     EEPROM.put(EEPROM_ADDR_CFG, config);

//     // Commit the changes to EEPROM
//     EEPROM.commit();
// }