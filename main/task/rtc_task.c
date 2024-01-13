// includes
#include "task.h"

#define RTC_TAG "RTC"

// declare all rtc functions in here
// function to set the time into the RTC
void set_ds3232_time(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // feedback
    ESP_LOGI(RTC_TAG, "Setting DS3232 time");

    // create a new i2c command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // start i2c communication
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);             // Set register pointer to 0x00 for seconds
 
    // set seconds, minutes, hours, and day
    i2c_master_write_byte(cmd, dec_to_bcd(seconds), true);
    i2c_master_write_byte(cmd, dec_to_bcd(minutes), true);
    i2c_master_write_byte(cmd, dec_to_bcd(hours), true);
    i2c_master_write_byte(cmd, 0x04, true);
    i2c_master_write_byte(cmd, dec_to_bcd(day), true);
    
    // Hardcoded writes for month and year (e.g., January 2024)
    i2c_master_write_byte(cmd, dec_to_bcd(0), true);    // Month (January)
    i2c_master_write_byte(cmd, dec_to_bcd(24), true);   // Century and tens of years
    
    // stop i2c communication
    i2c_master_stop(cmd);

    // execute the i2c command
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    // feedback
    ESP_LOGI(RTC_TAG, "DS3232 time set [%d:%d:%d:%d]", day, hours, minutes, seconds);

    // set the rtc time into ESP32 system time
    set_system_time_from_ds3232(seconds+1, minutes+1, hours+1, day, PRESET_MONTH, PRESET_YEAR);
}

// function to read the current time from the RTC (looped)
void read_ds3232_task(void *pvParameters) {
    // feedback
    ESP_LOGI(RTC_TAG, "Reading DS3232");

    // infinite loop in task
    while (1) {
        // create a new i2c command link
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        // start link
        i2c_master_start(cmd);

        // set register pointer to 0x00 for seconds
        i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, 0x00, true);

        // stop link
        i2c_master_stop(cmd);

        // send read command for seconds, minutes, hours, day, month, and year
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);

        // delete link (possibly to give RTC time to respond?)
        i2c_cmd_link_delete(cmd);

        // add delay to give DS3232 time to respond
        vTaskDelay(50 / portTICK_PERIOD_MS);

        // create a new i2c command link
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_READ, true);

        // extend the array size to accommodate day, month, and year
        uint8_t data[8];
        i2c_master_read(cmd, data, 7, I2C_MASTER_LAST_NACK);

        // stop link
        i2c_master_stop(cmd);

        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        // process data
        uint8_t seconds = ((data[0] & 0xF0) >> 4) * 10 + (data[0] & 0x0F);
        uint8_t minutes = ((data[1] & 0xF0) >> 4) * 10 + (data[1] & 0x0F);
        uint8_t hours = ((data[2] & 0xF0) >> 4) * 10 + (data[2] & 0x0F);
        uint8_t day = ((data[4] & 0xF0) >> 4) * 10 + (data[4] & 0x0F);
        uint8_t month = ((data[5] & 0x10) >> 4) * 10 + (data[5] & 0x0F);
        uint16_t year = ((data[6] & 0xF0) >> 4) * 10 + (data[6] & 0x0F) + 2000;

        // if in clock menu, do not sync time as it will read time from RTC and set into cfg, effectively overwriting what you are doing in clock menu
        if (nvm_cfg.flags.clock_flag == false) {
            // write rtc time to nvm_cfg struct for clock sync
            nvm_cfg.rtc.day = day;
            nvm_cfg.rtc.hour = hours;
            nvm_cfg.rtc.min = minutes;
            nvm_cfg.rtc.sec = seconds;

            // set read time from rtc to system time
            set_system_time_from_ds3232(seconds+1, minutes+1, hours+1, day, month+1, year); // +1 to compensate for using 1 through 12 or 0 through 11
        }

        // wait for 100 sec until rereading rtc (sync purposes)
        vTaskDelay(pdMS_TO_TICKS(100000));  
    }
}

// function to synchronize ESP system time with DS3232 RTC
void set_system_time_from_ds3232(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint16_t year) {
    // feedback
    ESP_LOGI(RTC_TAG, "Setting system time from DS3232 for synchronisation");

    // define local variables
    time_t now;             // time now
    struct tm timeinfo;     // struct to hold the read time

    // convert DS3232 readings to struct tm
    timeinfo.tm_year = year - 1900;         // Year - 1900
    timeinfo.tm_mon = month - 1;            // Month (0 - 11)
    timeinfo.tm_mday = day;                 // Day of the month (1 - 31)
    timeinfo.tm_hour = hours - 1;           // Hour (0 - 23)
    timeinfo.tm_min = minutes - 1;          // Minutes (0 - 59)
    timeinfo.tm_sec = seconds - 1;          // Seconds (0 - 59)

    // set the system time
    now = mktime(&timeinfo);

    // update the system time using settimeofday
    struct timeval tv = {
        .tv_sec = now,
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);
}

// helper function to convert decimal to BCD
uint8_t dec_to_bcd(uint8_t val) {
    // set the tens and ones place to the correct value
    return ((val / 10) << 4) | (val % 10);
}

// function to read the system time into cfg
void read_system_time_to_cfg() {
    // define local variables
    struct timeval tv;
    struct tm timeinfo;

    // get current (system) time
    gettimeofday(&tv, NULL);

    // convert to local time
    localtime_r(&tv.tv_sec, &timeinfo);

    // write local time to cfg
    nvm_cfg.rtc.day = timeinfo.tm_mday;
    nvm_cfg.rtc.hour = timeinfo.tm_hour;
    nvm_cfg.rtc.min = timeinfo.tm_min;
    nvm_cfg.rtc.sec = timeinfo.tm_sec;
}