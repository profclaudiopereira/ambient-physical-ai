idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        tab5_platform
        pahub
        env_iv
        oled_sh1107
        oled_context_presenter
        dlight
        ambient_console
        ambient_network
        semantic_event_receiver
)