// includes
#include "task.h"

SSD1306_t dev;

// declare all display functions in here
void init_display() {

#if CONFIG_I2C_INTERFACE
    ESP_LOGI("SSD1306", "INTERFACE is i2c");
    ESP_LOGI("SSD1306", "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
    ESP_LOGI("SSD1306", "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
    ESP_LOGI("SSD1306", "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif

#if CONFIG_SSD1306_128x64
    ESP_LOGI("SSD1306", "Panel is 128x64");
    ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
ssd1306_clear_screen(&dev, false);

}