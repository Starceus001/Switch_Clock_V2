set(component_srcs "ssd1306/ssd1306.c" "ssd1306/ssd1306_i2c.c" "ssd1306/ssd1306_spi.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "ssd1306/.")
