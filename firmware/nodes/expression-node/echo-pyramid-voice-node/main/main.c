/**
 * @file main.c
 * @brief Application entry point for the Echo Pyramid Voice Node.
 *
 * This firmware is derived from the validated Echo Pyramid + AtomS3R
 * laboratory baseline. The application currently provides:
 *
 * - Echo Pyramid RGB control through the STM32 I2C interface;
 * - UDP command reception;
 * - local conversion from simple UDP commands to semantic events;
 * - periodic runtime heartbeat logging;
 * - network initialization through the dedicated network_config module.
 *
 * Network responsibilities are intentionally isolated from this file so that
 * the application entry point remains focused on device orchestration.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_log.h"

#include "network_config.h"

#define UDP_PORT        5005
#define UDP_BUFFER_SIZE 128
#define WELCOME_PREFIX   "WELCOME|"
#define USER_NAME_SIZE   64

#define I2C_PORT        I2C_NUM_0
#define I2C_SDA_GPIO    38
#define I2C_SCL_GPIO    39
#define I2C_FREQ_HZ     100000

#define STM32_I2C_ADDR  0x1A

#define RGB1_BRIGHTNESS_REG_ADDR 0x10
#define RGB2_BRIGHTNESS_REG_ADDR 0x11
#define RGB1_STATUS_REG_ADDR     0x20
#define RGB2_STATUS_REG_ADDR     0x60

static const char *TAG = "echo_pyramid_voice";

static i2c_master_bus_handle_t i2c_bus = NULL;

/**
 * @brief Minimal semantic event representation used by the current baseline.
 *
 * The structure preserves the semantic separation between transport commands
 * and expression actions. It may later be replaced by the canonical runtime
 * event contract without changing the device-specific RGB functions.
 */
typedef struct {
    const char *type;
    const char *event;
    const char *target;
    const char *message;

    /*
     * Optional event parameter supplied by the Cognitive Runtime.
     *
     * For the personalized welcome contract, this field carries the
     * authenticated user's display name or profile identifier. It remains
     * NULL for legacy commands and for expression events that do not require
     * a parameter.
     */
    const char *parameter;
} semantic_event_t;

/* ================= I2C / RGB ================= */

/**
 * @brief Initializes the AtomS3R I2C master bus used by the Echo Pyramid.
 *
 * The Echo Pyramid exposes its RGB control registers through an onboard STM32
 * at address 0x1A. The bus is initialized once and retained for the lifetime
 * of the application.
 *
 * @return ESP_OK on success, otherwise an ESP-IDF error code.
 */
static esp_err_t init_i2c_bus(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&bus_config, &i2c_bus);
}

/**
 * @brief Writes one or more bytes to an Echo Pyramid STM32 register.
 *
 * A temporary device handle is created for each transaction. This preserves
 * the behavior of the validated laboratory baseline and keeps the transaction
 * contract explicit while the production driver is still being stabilized.
 *
 * @param dev_addr 7-bit I2C device address.
 * @param reg_addr Register address.
 * @param data Payload bytes.
 * @param len Number of payload bytes.
 *
 * @return ESP_OK on success, otherwise an ESP-IDF error code.
 */
static esp_err_t write_registers(uint8_t dev_addr,
                                 uint8_t reg_addr,
                                 const uint8_t *data,
                                 size_t len)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    i2c_master_dev_handle_t dev_handle = NULL;

    esp_err_t ret = i2c_master_bus_add_device(i2c_bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t buffer[16] = {0};

    if (len + 1 > sizeof(buffer)) {
        i2c_master_bus_rm_device(dev_handle);
        return ESP_ERR_INVALID_SIZE;
    }

    buffer[0] = reg_addr;
    memcpy(&buffer[1], data, len);

    ret = i2c_master_transmit(dev_handle, buffer, len + 1, 100);

    i2c_master_bus_rm_device(dev_handle);

    return ret;
}

/**
 * @brief Writes a single byte to an Echo Pyramid STM32 register.
 */
static esp_err_t write_register(uint8_t dev_addr,
                                uint8_t reg_addr,
                                uint8_t value)
{
    return write_registers(dev_addr, reg_addr, &value, 1);
}

/**
 * @brief Sets the brightness of one Echo Pyramid RGB channel.
 *
 * @param channel Echo Pyramid RGB channel number (1 or 2).
 * @param value Brightness percentage in the range 0 to 100.
 */
static esp_err_t set_brightness(uint8_t channel, uint8_t value)
{
    if (value > 100) {
        value = 100;
    }

    uint8_t reg_addr = (channel == 1)
                           ? RGB1_BRIGHTNESS_REG_ADDR
                           : RGB2_BRIGHTNESS_REG_ADDR;

    return write_register(STM32_I2C_ADDR, reg_addr, value);
}

/**
 * @brief Sets one RGB LED exposed by the Echo Pyramid STM32.
 *
 * The STM32 register layout uses BGR byte order followed by one reserved byte.
 */
static esp_err_t set_rgb(uint8_t channel,
                         uint8_t led_index,
                         uint8_t red,
                         uint8_t green,
                         uint8_t blue)
{
    if (led_index > 13) {
        led_index = 13;
    }

    uint8_t base_reg = (channel == 1)
                           ? RGB1_STATUS_REG_ADDR
                           : RGB2_STATUS_REG_ADDR;

    uint8_t reg_addr = base_reg + (led_index * 4);

    uint8_t data[4] = {
        blue,
        green,
        red,
        0x00
    };

    return write_registers(STM32_I2C_ADDR, reg_addr, data, sizeof(data));
}

/**
 * @brief Applies one RGB color to the LEDs selected by the validated baseline.
 *
 * This function intentionally preserves the laboratory LED selection until
 * the complete Echo Pyramid register map is formally incorporated.
 */
static void set_all_demo_leds(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(set_brightness(1, 100));
    ESP_ERROR_CHECK(set_brightness(2, 100));

    ESP_ERROR_CHECK(set_rgb(1, 0, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 3, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 7, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(1, 10, red, green, blue));

    ESP_ERROR_CHECK(set_rgb(2, 0, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 3, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 7, red, green, blue));
    ESP_ERROR_CHECK(set_rgb(2, 10, red, green, blue));
}

static void expression_rgb_welcome(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_WELCOME");
    set_all_demo_leds(0, 255, 0);
    ESP_LOGI(TAG, "RGB welcome reaction sent to Echo Pyramid.");
}

static void expression_rgb_red(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_RED");
    set_all_demo_leds(255, 0, 0);
}

static void expression_rgb_green(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_GREEN");
    set_all_demo_leds(0, 255, 0);
}

static void expression_rgb_blue(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_BLUE");
    set_all_demo_leds(0, 0, 255);
}

static void expression_rgb_off(void)
{
    ESP_LOGI(TAG, "Expression action: RGB_OFF");
    set_all_demo_leds(0, 0, 0);
}

/* ================= Semantic Event ================= */

/**
 * @brief Dispatches one local semantic event to a device expression action.
 *
 * This handler is deliberately transport-independent. UDP currently creates
 * these events, but future StackFlow integration may supply the same semantic
 * contract through a richer payload.
 */
static void handle_semantic_event(const semantic_event_t *event)
{
    ESP_LOGI(TAG, "Semantic Event received");
    ESP_LOGI(TAG, "type: %s", event->type);
    ESP_LOGI(TAG, "event: %s", event->event);
    ESP_LOGI(TAG, "target: %s", event->target);
    ESP_LOGI(TAG, "message: %s", event->message);

    if (event->parameter != NULL && event->parameter[0] != '\0') {
        ESP_LOGI(TAG, "parameter: %s", event->parameter);
    }

    if (strcmp(event->event, "welcome_researcher") == 0) {
        /*
         * The current firmware baseline has validated RGB expression but does
         * not yet expose a speech playback API in this source file. The user
         * parameter is therefore preserved and logged here so the upcoming
         * voice-confirmation implementation can consume it without changing
         * the UDP or semantic contracts again.
         */
        if (event->parameter != NULL && event->parameter[0] != '\0') {
            ESP_LOGI(TAG,
                     "Personalized welcome requested for authenticated user: %s",
                     event->parameter);
        }

        expression_rgb_welcome();
    } else if (strcmp(event->event, "rgb_red") == 0) {
        expression_rgb_red();
    } else if (strcmp(event->event, "rgb_green") == 0) {
        expression_rgb_green();
    } else if (strcmp(event->event, "rgb_blue") == 0) {
        expression_rgb_blue();
    } else if (strcmp(event->event, "rgb_off") == 0) {
        expression_rgb_off();
    } else {
        ESP_LOGW(TAG, "Unknown semantic event: %s", event->event);
    }
}

/**
 * @brief Converts the current simple UDP command format into semantic events.
 *
 * Supported commands:
 * - WELCOME
 * - WELCOME|<authenticated_user>
 * - RED
 * - GREEN
 * - BLUE
 * - OFF
 *
 * This compatibility parser preserves the validated E05.2 behavior while the
 * production node is prepared for the canonical runtime event contract.
 */
static void handle_udp_message(const char *msg)
{
    ESP_LOGI(TAG, "UDP message received: %s", msg);

    /*
     * Personalized welcome protocol
     * -----------------------------
     * Both formats are intentionally accepted:
     *
     *     WELCOME
     *     WELCOME|Claudio
     *
     * The first form preserves the validated legacy behavior. The second form
     * carries the authenticated user's name while keeping the embedded
     * protocol compact. The AX630C remains responsible for cognition and for
     * deciding which identity is allowed to reach this expression node.
     */
    if (strcmp(msg, "WELCOME") == 0 ||
        strncmp(msg, WELCOME_PREFIX, strlen(WELCOME_PREFIX)) == 0) {

        char user_name[USER_NAME_SIZE] = {0};
        const char *parameter = NULL;

        if (strncmp(msg, WELCOME_PREFIX, strlen(WELCOME_PREFIX)) == 0) {
            const char *received_name = msg + strlen(WELCOME_PREFIX);
            size_t received_length = strnlen(
                received_name,
                USER_NAME_SIZE
            );

            if (received_length > 0 && received_length < USER_NAME_SIZE) {
                memcpy(user_name, received_name, received_length);
                user_name[received_length] = '\0';
                parameter = user_name;
            } else if (received_length >= USER_NAME_SIZE) {
                /*
                 * Reject an oversized parameter instead of silently accepting
                 * a truncated identity. The legacy RGB welcome still runs,
                 * but the malformed name is not propagated.
                 */
                ESP_LOGW(TAG,
                         "Welcome parameter exceeds the supported size; "
                         "using legacy welcome behavior");
            }
        }

        semantic_event_t event = {
            .type = "semantic_event",
            .event = "welcome_researcher",
            .target = "expression_layer",
            .message = (parameter != NULL)
                           ? "Personalized welcome event received via UDP"
                           : "Welcome event received via UDP",
            .parameter = parameter
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "RED") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_red",
            .target = "expression_layer",
            .message = "Red event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "GREEN") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_green",
            .target = "expression_layer",
            .message = "Green event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "BLUE") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_blue",
            .target = "expression_layer",
            .message = "Blue event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else if (strcmp(msg, "OFF") == 0) {
        semantic_event_t event = {
            .type = "semantic_event",
            .event = "rgb_off",
            .target = "expression_layer",
            .message = "Off event received via UDP",
            .parameter = NULL
        };
        handle_semantic_event(&event);

    } else {
        ESP_LOGW(TAG, "Unknown UDP command: %s", msg);
    }
}

/* ================= UDP Task ================= */

/**
 * @brief Runs the UDP compatibility server used by the production baseline.
 *
 * The socket listens on all local interfaces at UDP port 5005. The port is
 * intentionally unchanged in this stabilization step. Alignment with the
 * canonical Runtime port will be performed as a separate integration change.
 */
static void udp_server_task(void *pv_parameters)
{
    (void)pv_parameters;

    char rx_buffer[UDP_BUFFER_SIZE];

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket");
        vTaskDelete(NULL);
        return;
    }

    int err = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (err < 0) {
        ESP_LOGE(TAG, "UDP socket bind failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "UDP server listening on port %d", UDP_PORT);

    while (true) {
        struct sockaddr_in source_addr;
        socklen_t source_addr_len = sizeof(source_addr);

        int len = recvfrom(sock,
                           rx_buffer,
                           sizeof(rx_buffer) - 1,
                           0,
                           (struct sockaddr *)&source_addr,
                           &source_addr_len);

        if (len < 0) {
            ESP_LOGE(TAG, "UDP recvfrom failed");
            continue;
        }

        rx_buffer[len] = '\0';
        handle_udp_message(rx_buffer);
    }
}

/* ================= Main ================= */

/**
 * @brief Initializes the Echo Pyramid Voice Node production baseline.
 *
 * Initialization order:
 * 1. Echo Pyramid I2C bus;
 * 2. network module with fixed IPv4 configuration;
 * 3. UDP semantic command server;
 * 4. periodic operational heartbeat.
 */
void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Expression Layer - Echo Pyramid Voice Node");
    ESP_LOGI(TAG, "Hardware: Echo Pyramid + AtomS3R");
    ESP_LOGI(TAG, "Target: ESP32-S3");
    ESP_LOGI(TAG, "Firmware stage: Production baseline derived from E05.2");
    ESP_LOGI(TAG, "Purpose: UDP semantic event reception and RGB expression");
    ESP_LOGI(TAG, "========================================");

    ESP_LOGI(TAG, "Chip cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);

    ESP_ERROR_CHECK(init_i2c_bus());
    ESP_ERROR_CHECK(network_init());

    BaseType_t task_created = xTaskCreate(
        udp_server_task,
        "udp_server_task",
        4096,
        NULL,
        5,
        NULL
    );

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UDP server task");
        return;
    }

    int counter = 0;

    while (true) {
        ESP_LOGI(TAG,
                 "Expression node heartbeat: %d | Wi-Fi: %s | IP: %s | UDP port: %d",
                 counter++,
                 network_is_connected() ? "connected" : "not connected",
                 network_get_ip(),
                 UDP_PORT);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
