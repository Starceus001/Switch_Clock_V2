// includes
#include "task.h"

#define RTC_TAG "RTC"

// struct tm synchronized_timeinfo;        // "TEST" look into what this does
// int timezone_offset_seconds;            // "TEST" look into what this does

// // declare all rtc functions in here
// // helper function to convert decimal to BCD
// uint8_t dec_to_bcd(uint8_t val) {
//     return ((val / 10) << 4) | (val % 10);
// }

// void read_ds3232_task() {
//     while (1) {
//         i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
//         // Set register pointer to 0x00 for seconds
//         i2c_master_start(cmd);
//         i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_WRITE, true);
//         i2c_master_write_byte(cmd, 0x00, true);
//         i2c_master_stop(cmd);
    
//         // Send read command for seconds, minutes, hours, and day
//         i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
//         i2c_cmd_link_delete(cmd);
    
//         vTaskDelay(50 / portTICK_PERIOD_MS);  // Wait for DS3232 to respond
    
//         cmd = i2c_cmd_link_create();
//         i2c_master_start(cmd);
//         i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_READ, true);
    
//         uint8_t data[4];
//         i2c_master_read(cmd, data, 4, I2C_MASTER_LAST_NACK);
//         i2c_master_stop(cmd);
    
//         i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
//         i2c_cmd_link_delete(cmd);
    
//         // Process data (data[0] is seconds, data[1] is minutes, data[2] is hours, data[3] is day)
//         uint8_t seconds = ((data[0] & 0xF0) >> 4) * 10 + (data[0] & 0x0F);
//         uint8_t minutes = ((data[1] & 0xF0) >> 4) * 10 + (data[1] & 0x0F);
//         uint8_t hours = ((data[2] & 0xF0) >> 4) * 10 + (data[2] & 0x0F);
//         uint8_t day = data[3];
    
//         printf("Time: %02d:%02d:%02d, Day: %02d\n", hours, minutes, seconds, day);
    
//         vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for one second
//     }
// }

// void set_ds3232_time(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
 
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (DS3232_ADDRESS << 1) | I2C_MASTER_WRITE, true);
//     i2c_master_write_byte(cmd, 0x00, true);  // Set register pointer to 0x00 for seconds
 
//     // Set seconds, minutes, hours, and day
//     i2c_master_write_byte(cmd, dec_to_bcd(seconds), true);
//     i2c_master_write_byte(cmd, dec_to_bcd(minutes), true);
//     i2c_master_write_byte(cmd, dec_to_bcd(hours), true);
//     i2c_master_write_byte(cmd, dec_to_bcd(day), true);
 
//     i2c_master_stop(cmd);
 
//     i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
//     i2c_cmd_link_delete(cmd);
// }

// void set_ds3232_to_system_time() {
//     time_t now;
//     struct tm timeinfo;
 
//     // Get current system time
//     time(&now);
//     localtime_r(&now, &timeinfo);
 
//     // Set DS3232 RTC with system time
//     set_ds3232_time(timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
// }

// // Function to synchronize system time with DS3232 RTC
// void synchronize_system_time_with_ds3232() {
//     time_t now;
//     struct tm timeinfo;
   
//     // Read DS3232 RTC time
//     // read_ds3232_task();
 
//     // Get current RTC time
//     time(&now);
//     localtime_r(&now, &timeinfo);
 
//     // Set system time with RTC time
//     // struct timeval tv = {
//     //     .tv_sec = now + (timeinfo.tm_sec - timeinfo.tm_gmtoff),
//     //     .tv_usec = 0
//     // };
//     // settimeofday(&tv, NULL);
// }