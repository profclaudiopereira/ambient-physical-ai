idf_component_register(
    SRCS
        "ambient_network.cpp"

    INCLUDE_DIRS
        "include"

    REQUIRES
        esp_common
        log
        nvs_flash
        esp_netif
        esp_event
        esp_wifi
)