/**
 * @file ws1850s.c
 * @brief WS1850S NFC reader driver implementation.
 *
 * This module implements the low-level communication routines required by the
 * Identity Node to operate the WS1850S RFID/NFC controller over the ESP-IDF
 * I2C master driver.
 *
 * Responsibilities include:
 *
 *  - Controller initialization.
 *  - Register access.
 *  - Bit manipulation helpers.
 *  - Card polling.
 *  - REQA request.
 *  - Anti-collision sequence.
 *  - UID acquisition.
 *
 * Higher application logic such as user identification, profile mapping and
 * semantic processing are intentionally implemented outside this driver.
 */

#include "ws1850s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief WS1850S register map used by this driver.
 *
 * Only the registers required for the Identity Node implementation are
 * defined here. Additional controller features can be added later without
 * changing the public API.
 */
#define COMMAND_REG        0x01
#define COM_IRQ_REG        0x04
#define ERROR_REG          0x06
#define FIFO_DATA_REG      0x09
#define FIFO_LEVEL_REG     0x0A
#define CONTROL_REG        0x0C
#define BIT_FRAMING_REG    0x0D
#define COLL_REG           0x0E
#define MODE_REG           0x11
#define TX_CONTROL_REG     0x14
#define TX_AUTO_REG        0x15
#define T_MODE_REG         0x2A
#define T_PRESCALER_REG    0x2B
#define T_RELOAD_REG_H     0x2C
#define T_RELOAD_REG_L     0x2D
#define VERSION_REG        0x37

#define PCD_IDLE           0x00
#define PCD_TRANSCEIVE     0x0C
#define PCD_SOFTRESET      0x0F

/**
 * @brief ISO/IEC 14443 Type A commands used during card discovery and
 * anti-collision.
 */
#define PICC_CMD_REQA      0x26
#define PICC_CMD_SEL_CL1   0x93

/**
 * @brief Confirms communication with the WS1850S controller.
 *
 * A minimal write transaction is performed to verify that the controller is
 * reachable on the configured I2C bus.
 *
 * This function is intended only for hardware probing and should not be used
 * as part of the normal NFC transaction flow.
 */
esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle)
{
    uint8_t dummy = 0x00;
    return i2c_master_transmit(dev_handle, &dummy, 1, 1000);
}

/**
 * @brief Reads a single controller register.
 *
 * This helper centralizes all register reads performed by the driver,
 * providing a consistent error handling path for higher-level operations.
 */
esp_err_t ws1850s_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t *value)
{
    if (value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_transmit_receive(dev_handle, &reg, 1, value, 1, 1000);
}

/**
 * @brief Writes a single controller register.
 *
 * The register address and value are transmitted together in one I2C
 * transaction.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to write.
 * @param value Value to be written.
 *
 * @return
 *      - ESP_OK if the register is written successfully.
 *      - An ESP-IDF error code if the I2C transaction fails.
 */
esp_err_t ws1850s_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return i2c_master_transmit(dev_handle, data, sizeof(data), 1000);
}

/**
 * @brief Sets selected bits in a controller register.
 *
 * Performs a read-modify-write operation by reading the current register
 * value, applying the supplied bit mask with a bitwise OR operation, and
 * writing the resulting value back to the controller.
 *
 * Bits that are not selected by the mask remain unchanged.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to modify.
 * @param mask Bit mask identifying the bits to set.
 *
 * @return
 *      - ESP_OK if the register is updated successfully.
 *      - An ESP-IDF error code if either the read or write transaction fails.
 */
esp_err_t ws1850s_set_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask)
{
    uint8_t value = 0;
    esp_err_t ret = ws1850s_read_register(dev_handle, reg, &value);
    if (ret != ESP_OK) {
        return ret;
    }

    return ws1850s_write_register(dev_handle, reg, value | mask);
}

/**
 * @brief Clears selected bits in a controller register.
 *
 * Performs a read-modify-write operation by reading the current register
 * value, clearing the bits selected by the supplied mask, and writing the
 * resulting value back to the controller.
 *
 * Bits that are not selected by the mask remain unchanged.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to modify.
 * @param mask Bit mask identifying the bits to clear.
 *
 * @return
 *      - ESP_OK if the register is updated successfully.
 *      - An ESP-IDF error code if either the read or write transaction fails.
 */
esp_err_t ws1850s_clear_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask)
{
    uint8_t value = 0;
    esp_err_t ret = ws1850s_read_register(dev_handle, reg, &value);
    if (ret != ESP_OK) {
        return ret;
    }

    return ws1850s_write_register(dev_handle, reg, value & (~mask));
}

/**
 * @brief Reads the controller version register.
 *
 * This function provides a named high-level operation for retrieving the
 * value stored in VERSION_REG while reusing the common register-access
 * implementation.
 *
 * Argument validation and I2C error propagation are delegated to
 * ws1850s_read_register().
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param version Output pointer that receives the version register value.
 *
 * @return
 *      - ESP_OK if the version register is read successfully.
 *      - ESP_ERR_INVALID_ARG if @p version is NULL.
 *      - Another ESP-IDF error code if the I2C transaction fails.
 */
esp_err_t ws1850s_read_version(i2c_master_dev_handle_t dev_handle, uint8_t *version)
{
    return ws1850s_read_register(dev_handle, VERSION_REG, version);
}

/**
 * @brief Initializes the WS1850S controller for ISO/IEC 14443 Type A
 * communication.
 *
 * The initialization sequence performs a software reset, waits for the
 * controller to stabilize, configures its internal timer, enables automatic
 * modulation behavior, selects the operating mode, and activates the antenna
 * driver outputs.
 *
 * Configuration writes are intentionally performed in a fail-fast sequence.
 * If any register transaction fails, initialization stops immediately and
 * returns the original ESP-IDF error code to the caller.
 *
 * The I2C bus and device handle must already be configured by the application.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 *
 * @return
 *      - ESP_OK if all initialization steps complete successfully.
 *      - An ESP-IDF error code from the first failed register transaction.
 */
esp_err_t ws1850s_init(i2c_master_dev_handle_t dev_handle)
{
    esp_err_t ret;

    // Reset the controller and allow its internal state to stabilize before
    // applying the operating configuration.
    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_SOFTRESET);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(50));

    // Configure the internal timer used to bound card communication
    // transactions and detect operations that do not complete.
    ret = ws1850s_write_register(dev_handle, T_MODE_REG, 0x8D);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_PRESCALER_REG, 0x3E);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_RELOAD_REG_L, 30);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_RELOAD_REG_H, 0);
    if (ret != ESP_OK) return ret;
    
    // Apply the communication mode and enable both antenna driver outputs
    // required for the RF field used during card polling.
    ret = ws1850s_write_register(dev_handle, TX_AUTO_REG, 0x40);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, MODE_REG, 0x3D);
    if (ret != ESP_OK) return ret;

    return ws1850s_set_bit_mask(dev_handle, TX_CONTROL_REG, 0x03);
}

/**
 * @brief Executes one bidirectional card communication transaction.
 *
 * This internal helper prepares the controller FIFO, loads the outbound
 * command bytes, starts the transceive operation, waits for the corresponding
 * interrupt, validates the controller status, and copies the received bytes
 * to the caller-provided buffer.
 *
 * The function does not interpret the NFC command or response semantics.
 * Higher-level operations such as REQA and anti-collision remain responsible
 * for validating the expected response length and content.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param send_data Buffer containing the bytes to transmit.
 * @param send_len Number of bytes available in @p send_data.
 * @param back_data Optional output buffer that receives response bytes.
 * @param back_len Input/output pointer. On entry, contains the capacity of
 *                 @p back_data; on return, contains the number of bytes made
 *                 available to the caller.
 * @param valid_bits Optional output pointer receiving the number of valid bits
 *                   in the final response byte.
 *
 * @return
 *      - ESP_OK if the transaction completes successfully.
 *      - ESP_ERR_INVALID_ARG if mandatory arguments are invalid.
 *      - ESP_ERR_TIMEOUT if the controller does not complete the operation
 *        within the polling interval.
 *      - ESP_FAIL if the controller reports a protocol-level error.
 *      - Another ESP-IDF error code if an I2C operation fails.
 */
static esp_err_t ws1850s_transceive(
    i2c_master_dev_handle_t dev_handle,
    const uint8_t *send_data,
    uint8_t send_len,
    uint8_t *back_data,
    uint8_t *back_len,
    uint8_t *valid_bits)
{
    if (send_data == NULL || send_len == 0 || back_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;

    // Return the controller to an idle state, clear pending interrupt flags,
    // and flush the FIFO before loading a new transaction.
    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, COM_IRQ_REG, 0x7F);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, FIFO_LEVEL_REG, 0x80);
    if (ret != ESP_OK) return ret;

    // Load the complete outbound frame into the controller FIFO before
    // starting the transceive command.
    for (uint8_t i = 0; i < send_len; i++) {
        ret = ws1850s_write_register(dev_handle, FIFO_DATA_REG, send_data[i]);
        if (ret != ESP_OK) return ret;
    }

    // Select the transceive command and assert StartSend only after the
    // outbound frame has been fully staged in the FIFO.
    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_TRANSCEIVE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);
    if (ret != ESP_OK) return ret;

    bool irq_completed = false;

    // Poll the communication interrupt flags for up to approximately 200 ms.
    // The delay avoids continuously occupying the task while the controller
    // completes RF transmission or reception.
    for (int i = 0; i < 100; i++) {
        uint8_t irq = 0;

        ret = ws1850s_read_register(dev_handle, COM_IRQ_REG, &irq);
        if (ret != ESP_OK) {
            ws1850s_clear_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);
            ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
            return ret;
        }

        if (irq & 0x30) {
            irq_completed = true;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(2));
    }

    // Clear StartSend regardless of whether completion was detected, leaving
    // the framing register ready for the next NFC operation.
    ret = ws1850s_clear_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);
    if (ret != ESP_OK) {
        ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
        return ret;
    }

    // A missing completion interrupt is reported as a transaction timeout.
    // Reset the reported response length so callers cannot consume stale data.
    if (!irq_completed) {
        ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
        *back_len = 0;
        return ESP_ERR_TIMEOUT;
    }

    // Inspect the controller error flags before exposing any received payload
    // to the higher-level NFC operation.
    uint8_t error = 0;
    ret = ws1850s_read_register(dev_handle, ERROR_REG, &error);
    if (ret != ESP_OK) return ret;

    if (error & 0x13) {
        return ESP_FAIL;
    }

    // Read the response size and final-byte bit count before draining the
    // receive FIFO.
    uint8_t fifo_level = 0;
    ret = ws1850s_read_register(dev_handle, FIFO_LEVEL_REG, &fifo_level);
    if (ret != ESP_OK) return ret;

    uint8_t control = 0;
    ret = ws1850s_read_register(dev_handle, CONTROL_REG, &control);
    if (ret != ESP_OK) return ret;

    if (valid_bits != NULL) {
        *valid_bits = control & 0x07;
    }

    uint8_t count = fifo_level;
 
    // Respect the caller-provided buffer capacity. Excess FIFO bytes are not
    // copied by this transaction, preventing writes beyond the output buffer.
    if (back_data != NULL) {
        if (count > *back_len) {
            count = *back_len;
        }

        for (uint8_t i = 0; i < count; i++) {
            ret = ws1850s_read_register(dev_handle, FIFO_DATA_REG, &back_data[i]);
            if (ret != ESP_OK) return ret;
        }
    }

    *back_len = count;
    return ESP_OK;
}

/**
 * @brief Checks whether an ISO/IEC 14443 Type A card is present.
 *
 * Sends a seven-bit REQA command and evaluates the two-byte ATQA response
 * expected from a compatible card.
 *
 * The absence of a card is part of normal polling behavior. Therefore,
 * transaction timeout, protocol failure, or an equivalent not-found result
 * are converted into ESP_OK with @p present set to false.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param present Output pointer set to true when a valid ATQA response is
 *                received, or false when no compatible card is detected.
 *
 * @return
 *      - ESP_OK when polling completes, including the normal no-card case.
 *      - ESP_ERR_INVALID_ARG if @p present is NULL.
 *      - Another ESP-IDF error code if controller communication fails in a
 *        way that is not interpreted as card absence.
 */
esp_err_t ws1850s_card_present(i2c_master_dev_handle_t dev_handle, bool *present)
{
    if (present == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *present = false;

    // REQA is transmitted as a seven-bit short frame. A valid Type A card
    // responds with a two-byte ATQA value.
    uint8_t send_data[1] = {PICC_CMD_REQA};
    uint8_t back_data[2] = {0};
    uint8_t back_len = sizeof(back_data);
    uint8_t valid_bits = 0;

    esp_err_t ret = ws1850s_write_register(dev_handle, BIT_FRAMING_REG, 0x07);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_transceive(
        dev_handle,
        send_data,
        sizeof(send_data),
        back_data,
        &back_len,
        &valid_bits);

    // In NFC polling, no response means no card, not a critical failure.
    if (ret == ESP_ERR_TIMEOUT || ret == ESP_ERR_NOT_FOUND || ret == ESP_FAIL) {
        *present = false;
        return ESP_OK;
    }

    if (ret != ESP_OK) {
        return ret;
    }

    // Accept card presence only when the response matches the complete
    // two-byte ATQA format expected from the REQA command.
    if (back_len == 2 && valid_bits == 0) {
        *present = true;
    }

    return ESP_OK;
}

/**
 * @brief Reads a four-byte UID through the cascade-level-one anti-collision
 * sequence.
 *
 * This function assumes that card presence has already been confirmed by the
 * caller. It sends the SEL CL1 anti-collision command, validates the returned
 * block check character, and copies the first four UID bytes to the caller.
 *
 * The current implementation supports only the four-byte UID form returned
 * directly at cascade level one. Additional cascade levels are not processed.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param uid Output buffer with capacity for at least four bytes.
 * @param uid_len Output pointer receiving the number of valid UID bytes.
 *
 * @return
 *      - ESP_OK if a valid four-byte UID is acquired.
 *      - ESP_ERR_INVALID_ARG if @p uid or @p uid_len is NULL.
 *      - ESP_ERR_NOT_FOUND if the card does not provide a usable
 *        anti-collision response.
 *      - ESP_ERR_INVALID_CRC if the returned BCC does not match the UID bytes.
 *      - Another ESP-IDF error code if controller communication fails.
 */
esp_err_t ws1850s_read_uid(i2c_master_dev_handle_t dev_handle, uint8_t *uid, uint8_t *uid_len)
{
    if (uid == NULL || uid_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *uid_len = 0;

    // Do not call ws1850s_init() here. Initialization is owned by setup/recovery.
    // Do not call ws1850s_card_present() here. The NFC task already reached READ_UID
    // after a positive poll; a second REQA can disturb the anticollision sequence.

    vTaskDelay(pdMS_TO_TICKS(10));

    // Prepare cascade level one anti-collision and request the complete UID
    // response beginning at bit position 0.
    esp_err_t ret = ws1850s_write_register(dev_handle, COLL_REG, 0x80);
    if (ret != ESP_OK) return ret;

    uint8_t send_data[2] = {PICC_CMD_SEL_CL1, 0x20};
    uint8_t back_data[10] = {0};
    uint8_t back_len = sizeof(back_data);
    uint8_t valid_bits = 0;

    ret = ws1850s_write_register(dev_handle, BIT_FRAMING_REG, 0x00);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_transceive(
        dev_handle,
        send_data,
        sizeof(send_data),
        back_data,
        &back_len,
        &valid_bits);

    // Normalize missing or unusable card responses into a single not-found
    // result for the NFC task state machine.
    if (ret == ESP_ERR_TIMEOUT || ret == ESP_ERR_NOT_FOUND || ret == ESP_FAIL) {
        return ESP_ERR_NOT_FOUND;
    }

    if (ret != ESP_OK) {
        return ret;
    }

    // Cascade level one must provide four UID bytes followed by one BCC byte.
    if (back_len < 5) {
        return ESP_ERR_NOT_FOUND;
    }

    uint8_t bcc = back_data[0] ^ back_data[1] ^ back_data[2] ^ back_data[3];

    // Validate the block check character before exposing the UID to the
    // application layer.
    if (bcc != back_data[4]) {
        return ESP_ERR_INVALID_CRC;
    }

    uid[0] = back_data[0];
    uid[1] = back_data[1];
    uid[2] = back_data[2];
    uid[3] = back_data[3];
    *uid_len = 4;

    return ESP_OK;
}
