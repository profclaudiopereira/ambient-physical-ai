idf_component_register(
    SRCS
        "cognitive_runtime_console.c"
        "ui/ui_manager.c"
        "ui/screen_runtime.c"
        "ui/screen_state.c"
        "ui/screen_health.c"
        "ui/screen_network.c"
        "ui/screen_log.c"
        "runtime/runtime_status.c"
        "transport/runtime_uart.c"

    INCLUDE_DIRS
        "."
        "ui"
        "runtime"
        "transport"

    REQUIRES
        m5stack_core_s3
        json
        esp_driver_uart
)