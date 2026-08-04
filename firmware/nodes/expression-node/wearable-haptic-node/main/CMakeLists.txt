idf_component_register(
    SRCS
        "stickc_plus2_rgb_node.cpp"
        "expression_processor.c"
        "rgb_controller.cpp"
        "rgb_effects.c"
        "semantic_consumer.c"
        "semantic_receiver.c"
        "wifi_station.c"
        "vibrator.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        M5Unified
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        json
        driver
)