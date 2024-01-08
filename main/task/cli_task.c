// includes
#include "task.h"

#define CLI_TAG "CLI"

// declare all cli functions in here
// read the cli constantly in main while loop
void read_cli_constant() {
    // define local variables
    size_t available_bytes = 0;

    // get the length of the buffered data on the uart for future reading
    uart_get_buffered_data_len(UART_NUM, &available_bytes);
    
    // check if there is data available (if we don't, big errors...)
    if (available_bytes > 0) {
        // allocate memory for the data
        uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

        // read the data from the uart
        int buf = uart_read_bytes(UART_NUM, data, BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        
        // check if we have received data (is double but if we don't check soon, we can get into problems)
        if (buf > 0) {
            // null-terminate the received data
            data[buf] = 0;
            
            // process the received command
            handle_command((const char*)data);
        }
        // Free allocated memory
        free(data);
    }
    // delete task
    vTaskDelete(NULL);
}

// function to handle uart commands
void handle_command(const char* command) {
    // define local variables
    char trimmed_command[64];   // Adjust the size as needed
    // Copy the command to a local buffer and trim whitespace
    strncpy(trimmed_command, command, sizeof(trimmed_command) - 1);
    trim_whitespace(trimmed_command);

    // Check if the command is "set_time"
    if (strncmp(trimmed_command, "set_time", 8) == 0) {
        // call function to run command code
        cli_command_set_time(trimmed_command);

    // check if the command is "timer_rep"
    } else if (strncmp(trimmed_command, "timer_rep", 10) == 0) {
        // call function to run command code
        cli_command_timer_rep(trimmed_command);
        
    // check if the command is "cfg_print"
    } else if (strcmp(trimmed_command, "cfg_print", 10) == 0) {
        // call function to run command code
        cli_command_cfg_print(trimmed_command);

    }
    // "TEST" to continue adding all other functions over cli
    /*
    - set timers individually with all settings (seperate commands?)
    */ 
    // check if the command is "help"
    else if (strcmp(trimmed_command, "help") == 0) {
        // call function to run command code
        cli_command_help(trimmed_command);

    } else {
        ESP_LOGW(CLI_TAG, "Unknown command: %s", trimmed_command);
    }
}

// command for clock time set
void cli_command_set_time(const char* command) {
    // feedback
        ESP_LOGI(CLI_TAG, "Executing Set_time command");

        // skip "Set_time" part to get to data
        const char* time_str = trimmed_command + 8;

        // parse the values using sscanf
        uint8_t day, hours, minutes, seconds;
        int result = sscanf(time_str, "%hhu,%hhu,%hhu,%hhu", &day, &hours, &minutes, &seconds);

        // Check if we have parsed all 4 values
        if (result == 4) {
            // call the function to set the time
            set_ds3232_time(day, hours, minutes, seconds);

            // set the time in nvm_cfg
            nvm_cfg.rtc.day = day;
            nvm_cfg.rtc.hour = hours;
            nvm_cfg.rtc.min = minutes;
            nvm_cfg.rtc.sec = seconds;
            
            // feedback
            ESP_LOGI(CLI_TAG, "Done executing Set_time command");
        } else {
            ESP_LOGW(CLI_TAG, "Invalid Set_time command format");
        }
}

// command for timer repeat
cli_command_timer_rep(const char* command) {
    // feedback
        ESP_LOGI(CLI_TAG, "Executing Timer_Start_periodic command");

        // skip "timer_rep" part to get to data
        const char* time_str = trimmed_command + 10;

        // parse the argument using sscanf
        uint16_t milliseconds;
        int result = sscanf(time_str, "%hu", &milliseconds);

        // check if we have parsed the value
        if (result == 1) {
            // call the function to start periodic timer
            // Timer_Start_periodic(milliseconds);                  // "TEST" to be implemented

            // feedback
            ESP_LOGI(CLI_TAG, "Done executing Timer_Start_periodic with %hu milliseconds", milliseconds);
        } else {
            ESP_LOGW(CLI_TAG, "Invalid Timer_Start_periodic command format");
        }
}

// command for cfg printing
cli_command_cfg_print(const char* command) {
    // feedback
    ESP_LOGI(CLI_TAG, "Executing cfg_print command");

    // print the cfg and nvm_cfg
    cfg_print();

    // feedback
    ESP_LOGI(CLI_TAG, "Done executing cfg_print command");
}

// command for help
cli_command_help(const char* command) {
    // print list of available commands
    ESP_LOGI(CLI_TAG, "---------------------------------------------------------------------------------------------------------");
    ESP_LOGI(CLI_TAG, "| Available commands:                                                                                    |");
    ESP_LOGI(CLI_TAG, "|  - timer_rep <ms>     Dit is een test functie om de repetetief te laten schakelen op custom tijd       |");
    ESP_LOGI(CLI_TAG, "|  - set_time <day>,<hours>,<minutes>,<seconds>                                                          |");
    ESP_LOGI(CLI_TAG, "|  - cfg_print                                                                                           |");
    ESP_LOGI(CLI_TAG, "|  - help                                                                                                |");
    ESP_LOGI(CLI_TAG, "---------------------------------------------------------------------------------------------------------");
}