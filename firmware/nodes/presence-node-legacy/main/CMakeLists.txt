idf_component_register(
    SRCS "presence-node.c"
    INCLUDE_DIRS "."
    REQUIRES vl53l0x esp_driver_i2c
)