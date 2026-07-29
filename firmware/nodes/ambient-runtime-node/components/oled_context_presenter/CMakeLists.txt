idf_component_register(
    SRCS "oled_context_presenter.c"
    INCLUDE_DIRS "include"
    REQUIRES oled_sh1107 semantic_event_receiver
)
