// includes
#include "task.h"

#define CLI_TAG "CLI"

// read the cli constantly in main while loop
void read_cli_constant() {
    // define buffer with set size only on first run
    char buf[BUF_SIZE];

    // clear buffer to start with a buffer full of 0's
    memset(buf, 0, sizeof(buf));

    // flag to indicate if the buffer is full
    int buffer_full = 0;

    // loop to constantly read UART buffer into my buffer
    while (1) {
        // use scanf to read data from UART into holder
        char holder[BUF_SIZE];  // declare holder variable
        if (scanf("%s", holder) == 1) {
            // check if the buffer is full
            if (strlen(buf) + strlen(holder) < BUF_SIZE - 1) {
                strcat(buf, holder);  // append holder to the end of buf
            } else {
                // handle buffer full error
                ESP_LOGE(CLI_TAG, "Buffer is full, emptying buffer");
                memset(buf, 0, sizeof(buf));  // optionally clear the buffer or take other actions
                buffer_full = 1;  // set the flag to indicate buffer full
            }

            // check if a newline is found in the entire buffer
            if (!buffer_full) {
                char *newline_pos = strchr(buf, ';');
                if (newline_pos != NULL) {
                    *newline_pos = '\0';      // null-terminate at the newline position
                    handle_command(buf);      // process the command

                    // move the remaining data to the front of the buffer
                    memmove(buf, newline_pos + 1, strlen(newline_pos + 1) + 1);
                }
            }

            // reset the buffer full flag
            buffer_full = 0;
        }

        // wait one second until reading the buffer again
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// function to handle uart commands
void handle_command(char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Received command: %s", command);

    // define local variables
    char trimmed_command[64];   // Adjust the size as needed
    char given_command[32];
    // Copy the command to a local buffer and trim finalizer (;)
    strncpy(trimmed_command, command, sizeof(trimmed_command) - 1);
    trim_whitespace(trimmed_command);

    // get command from leftover (without ;) up to (-) to get command name
    // get the first (-) position (if any)
    char *hyphen_position = strchr(trimmed_command, '-');

    // check if hyphen was found
    if (hyphen_position != NULL) {
        // calculate the length up to the hyphen
        size_t length_up_to_hyphen = hyphen_position - trimmed_command;

        // copy the substring up to the hyphen to the command variable
        strncpy(given_command, trimmed_command, length_up_to_hyphen);
        given_command[length_up_to_hyphen] = '\0';  // Null-terminate the string
    } else {
        // no (-) was found, this could be a normal command so this is not needed
    }

    // check if the command is "set_time"
    if (strncmp(given_command, "set_time", 9) == 0) {
        // call function to run command code
        cli_command_set_time(trimmed_command);

    // check if the command is "timer_set"
    } else if (strncmp(given_command, "timer_set", 10) == 0) {
        // call function to run command code
        cli_command_timer_set(trimmed_command);

    // check if the command is "timers_all_rep"
    } else if (strncmp(given_command, "timers_all_rep", 15) == 0) {
        // call function to run command code
        cli_command_timers_all_rep(trimmed_command);
        
    // check if the command is "cfg_print"
    } else if (strncmp(trimmed_command, "cfg_print", 10) == 0) {
        // call function to run command code
        cli_command_cfg_print(trimmed_command);

    }
    
    // check if the command is "help"
    else if (strncmp(trimmed_command, "help", 5) == 0) {
        // call function to run command code
        cli_command_help(trimmed_command);

    } else {
        ESP_LOGW(CLI_TAG, "Unknown command: %s", trimmed_command);
    }
}

// command for clock time set
void cli_command_set_time(char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Executing Set_time command");

    // skip "Set_time" part to get to data
    const char* time_str = command + 8;

    // parse the values using sscanf
    uint8_t day, hours, minutes, seconds;
    int result = sscanf(time_str, "%hhu,%hhu,%hhu,%hhu", &day, &hours, &minutes, &seconds);

    // Check if we have parsed all 4 values
    if (result == 4) {
        // variable to determine if any data reading was invalid
        uint8_t ret = 0;

        // set the time in nvm_cfg
        if (day >= 1 && day <= 7) nvm_cfg.rtc.day = day;
        else ret = -1;
        if (hours >= 0 && hours <= 23) nvm_cfg.rtc.hour = hours;
        else ret = -1;
        if (minutes >= 0 && minutes <= 59) nvm_cfg.rtc.min = minutes;
        else ret = -1;
        if (seconds >= 0 && seconds <= 59) nvm_cfg.rtc.sec = seconds;
        else ret = -1;

        if (ret == -1) {
            // error log and leave function, data invalid
            ESP_LOGW(CLI_TAG, "Invalid Set_time command format");

            return;
        }

        // set the time in ds3232
        set_ds3232_time(day, hours, minutes, seconds);
        
        // feedback
        ESP_LOGI(CLI_TAG, "Done executing Set_time command");
    } else {
        ESP_LOGW(CLI_TAG, "Invalid Set_time command format");
    }
}

// command for setting a specific timer
void cli_command_timer_set(char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Executing timer_set command");

    // skip "timer_rep" part to get to data
    const char* time_str = command + 10;

    // parse the argument using sscanf
    uint16_t timer_choice, active, set_value, set_day, set_hours, set_minutes, set_seconds, set_milliseconds, rep_timer, rep_hours, rep_minutes, rep_seconds, rep_milliseconds;
    int result = sscanf(time_str, "%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu", &timer_choice-1, &active, &set_value, &set_day, &set_hours, &set_minutes, &set_seconds, &set_milliseconds, &rep_timer, &rep_hours, &rep_minutes, &rep_seconds, &rep_milliseconds);
    
    // check if we have parsed 13 values
    if (result == 12) {
        // variable to determine if any data reading was invalid
        uint8_t ret = 0;

        // write all to cfg if check passed
        if (active == 0 || active == 1) cfg.timers[timer_choice].timer_active = active; 
        else ret = -1;
        if (set_value == 0 || set_value == 1) cfg.timers[timer_choice].set_value = set_value; 
        else ret = -1;
        if (set_day >= 1 && set_day <= 7) cfg.timers[timer_choice].set_day = set_day;
        else ret = -1;
        if (set_hours >= 0 && set_hours <= 23) cfg.timers[timer_choice].set_hour = set_hours; 
        else ret = -1;
        if (set_minutes >= 0 && set_minutes <= 59) cfg.timers[timer_choice].set_min = set_minutes; 
        else ret = -1;
        if (set_seconds >= 0 && set_seconds <= 59) cfg.timers[timer_choice].set_sec = set_seconds; 
        else ret = -1;
        if (set_milliseconds >= 0 && set_milliseconds < 999) cfg.timers[timer_choice].set_ms = set_milliseconds; 
        else ret = -1;
        if (rep_timer == 0 || rep_timer == 1) cfg.timers[timer_choice].repeat_timer = rep_timer; 
        else ret = -1;
        if (rep_hours >= 0 && rep_hours <= 23) cfg.timers[timer_choice].repeat_interval_hour = rep_hours; 
        else ret = -1;
        if (rep_minutes >= 0 && rep_minutes <= 59) cfg.timers[timer_choice].repeat_interval_min = rep_minutes; 
        else ret = -1;
        if (rep_seconds >= 0 && rep_seconds <= 59) cfg.timers[timer_choice].repeat_interval_sec = rep_seconds; 
        else ret = -1;
        if (rep_milliseconds >= 0 && rep_milliseconds <= 999) cfg.timers[timer_choice].repeat_interval_ms = rep_milliseconds; 
        else ret = -1;

        if (ret == -1) {
            // error log and leave function, data invalid
            ESP_LOGW(CLI_TAG, "Invalid timer_set command format");

            return;
        }

        // calculate repeat interval to ms
        cfg.timers[nvm_cfg.flags.chosen_timer].interval_in_ms = cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_hour * 3600000 +
                                        cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_min * 60000 +
                                        cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_sec * 1000 +
                                        cfg.timers[nvm_cfg.flags.chosen_timer].repeat_interval_ms;

        // feedback
        ESP_LOGI(CLI_TAG, "Done executing timer_set command");
    } else {
        ESP_LOGW(CLI_TAG, "Invalid timer_set command format");
    }
}

// command for setting all timers on on repeat for given ms
// void cli_command_timers_all_rep(char* command) {
//     // feedback
//     ESP_LOGI(CLI_TAG, "Executing timers_all_rep command");

//     // skip "timers_all_rep" part to get to data
//     const char* time_str = command + 15;

//     // parse the argument using sscanf
//     uint32_t milliseconds;
//     int result = sscanf(time_str, "%lu", &milliseconds);

//     // check if value is valid
//     if (milliseconds > 0 && milliseconds < 86400000) {        // 86.400.000 is de top value of uint32_t so we can at max do 86.399.999 ms
//         // Check if we have parsed all 1 values
//         if (result == 1) {
//             // save interval in milliseconds to cfg of each timer
//             for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
//                 ESP_LOGE(CLI_TAG, "Setting timer [%i] repeat_interval_ms to %lu", i, milliseconds);
//                 cfg.timers[i].interval_in_ms = milliseconds; 
//             }
//             // call function to turn on all timers on repeat
//             // timer_start_periodic_all();
//         }
//     }
//     else {
//         // feedback
//         ESP_LOGW(CLI_TAG, "Invalid timers_all_rep command format");
//     }
// }

// command for setting all timers on on repeat for given ms
void cli_command_timers_all_rep(char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Executing timers_all_rep command");

    // skip "timers_all_rep" part to get to data
    const char* time_str = command + 15;

    // parse the argument using sscanf
    uint32_t milliseconds;
    int result = sscanf(time_str, "%lu", &milliseconds);

    // check if value is valid
    if (milliseconds > 0 && milliseconds < 86400000) {        // 86.400.000 is de top value of uint32_t so we can at max do 86.399.999 ms
        // Check if we have parsed all 1 values
        if (result == 1) {
            // save interval in milliseconds to cfg of each timer
            for (uint8_t i = 0; i <= MAX_TIMER_COUNT; i++) {
                ESP_LOGE(CLI_TAG, "Setting timer [%i] repeat_interval_ms to %lu", i, milliseconds);
                cfg.timers[i].interval_in_ms = milliseconds; 
            }
            // call function to turn on all timers on repeat
            // timer_start_periodic_all();
        }
    }
    else {
        // feedback
        ESP_LOGW(CLI_TAG, "Invalid timers_all_rep command format");
    }
}

// command for cfg printing
void cli_command_cfg_print(char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Executing cfg_print command");

    // print the cfg and nvm_cfg
    cfg_print();

    // feedback
    ESP_LOGI(CLI_TAG, "Done executing cfg_print command");
}

uint16_t timer_choice, active, set_value, set_day, set_hours, set_minutes, set_seconds, set_milliseconds, rep_timer, rep_hours, rep_minutes, rep_seconds, rep_milliseconds;

// command for help
void cli_command_help(char* command) {
    // print list of available commands
    ESP_LOGI(CLI_TAG, "--------------------------------------------------------------------------------------------------------------------------");
    ESP_LOGI(CLI_TAG, "| Available commands:                                                                                                    |");
    ESP_LOGI(CLI_TAG, "|  - set_time-day,hours,minutes,seconds;                                                                                 |");
    ESP_LOGI(CLI_TAG, "|  - timer_set-timer,on/off,output_value,set_day,set_hour,set_min,set_sec,set_ms,repeat,rep_hour,rep_min,rep_sec,rep_ms; |"); 
    ESP_LOGI(CLI_TAG, "|  - timers_all_rep-ms;                                                                                                  |");
    ESP_LOGI(CLI_TAG, "|  - cfg_print;                                                                                                          |");
    ESP_LOGI(CLI_TAG, "|  - help;                                                                                                               |");
    ESP_LOGI(CLI_TAG, "|                                                                                                                        |");
    ESP_LOGI(CLI_TAG, "| When writing any command, replace each value designated 'timer' with the value you wish to add.                        |");
    ESP_LOGI(CLI_TAG, "| Example: set_time-5,12,45,54;                                                                                          |");
    ESP_LOGI(CLI_TAG, "--------------------------------------------------------------------------------------------------------------------------");
}

// function to remove whitespaces from cli commands
void trim_whitespace(char* str) {
    // trim leading whitespaces
    while (isspace((unsigned char)*str)) {
        str++;
    }
 
    // trim trailing whitespaces
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
 
    // null-terminate the trimmed string
    *(end + 1) = '\0';
}