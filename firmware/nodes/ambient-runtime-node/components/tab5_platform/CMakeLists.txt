idf_component_register(
    SRCS "tab5_platform.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        driver
        esp_lcd
        esp_driver_ledc
        esp_driver_gpio
        esp_lcd_st7121
        esp_hw_support
        heap
)