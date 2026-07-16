idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        ld2410
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        lwip
)
