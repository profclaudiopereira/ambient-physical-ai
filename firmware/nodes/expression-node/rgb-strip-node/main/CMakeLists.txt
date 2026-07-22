idf_component_register(
    SRCS
        "atom_neopixel_grove.c"
        "expression_processor.c"
        "rgb_controller.c"
        "rgb_effects.c"
        "semantic_consumer.c"
        "semantic_receiver.c"
        "wifi_station.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        led_strip
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        json
)