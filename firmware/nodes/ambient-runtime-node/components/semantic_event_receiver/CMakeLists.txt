idf_component_register(
    SRCS
        "semantic_event_receiver.cpp"

    INCLUDE_DIRS
        "include"

    REQUIRES
        esp_common
        log
)