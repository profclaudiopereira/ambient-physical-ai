/**
 * @file ws1850s.h
 * @brief Public interface for the WS1850S NFC reader driver.
 *
 * This component provides the hardware abstraction used by the Identity Node
 * to communicate with the WS1850S controller through the ESP-IDF I2C master
 * driver.
 *
 * The public API exposes device probing, register access, initialization,
 * card detection, and UID acquisition while keeping protocol-specific details
 * encapsulated in the implementation file.
 */

#ifndef WS1850S_H
#define WS1850S_H
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default I2C address of the WS1850S controller.
 */
#define WS1850S_I2C_ADDRESS 0x28

/**
 * @brief Verifies communication with the WS1850S device.
 *
 * Performs a basic I2C transaction to confirm that the controller is
 * reachable through the supplied device handle.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 *
 * @return
 *      - ESP_OK if the device responds successfully.
 *      - An ESP-IDF error code if communication fails.
 */
esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle);

/**
 * @brief Reads one register from the WS1850S controller.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to read.
 * @param value Output pointer that receives the register value.
 *
 * @return
 *      - ESP_OK if the register is read successfully.
 *      - ESP_ERR_INVALID_ARG if an argument is invalid.
 *      - Another ESP-IDF error code if the I2C transaction fails.
 */
esp_err_t ws1850s_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t *value);

/**
 * @brief Writes one register in the WS1850S controller.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to write.
 * @param value Value to be written.
 *
 * @return
 *      - ESP_OK if the register is written successfully.
 *      - An ESP-IDF error code if the I2C transaction fails.
 */
esp_err_t ws1850s_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t value);

/**
 * @brief Sets selected bits in a WS1850S register.
 *
 * Performs a read-modify-write operation so that bits outside the supplied
 * mask remain unchanged.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to modify.
 * @param mask Bit mask identifying the bits to set.
 *
 * @return
 *      - ESP_OK if the register is updated successfully.
 *      - An ESP-IDF error code if a read or write transaction fails.
 */
esp_err_t ws1850s_set_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask);

/**
 * @brief Clears selected bits in a WS1850S register.
 *
 * Performs a read-modify-write operation so that bits outside the supplied
 * mask remain unchanged.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param reg Address of the register to modify.
 * @param mask Bit mask identifying the bits to clear.
 *
 * @return
 *      - ESP_OK if the register is updated successfully.
 *      - An ESP-IDF error code if a read or write transaction fails.
 */
esp_err_t ws1850s_clear_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask);

/**
 * @brief Reads the controller version register.
 *
 * The returned value can be used during initialization or diagnostics to
 * confirm that the expected WS1850S-compatible controller is responding.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param version Output pointer that receives the version register value.
 *
 * @return
 *      - ESP_OK if the version is read successfully.
 *      - ESP_ERR_INVALID_ARG if the output pointer is invalid.
 *      - Another ESP-IDF error code if the I2C transaction fails.
 */
esp_err_t ws1850s_read_version(i2c_master_dev_handle_t dev_handle, uint8_t *version);

/**
 * @brief Initializes the WS1850S controller for card communication.
 *
 * Applies the register configuration required before card presence detection
 * and UID acquisition can be performed.
 *
 * The I2C bus and device handle must already have been created by the
 * application before this function is called.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 *
 * @return
 *      - ESP_OK if initialization completes successfully.
 *      - An ESP-IDF error code if a configuration transaction fails.
 */
esp_err_t ws1850s_init(i2c_master_dev_handle_t dev_handle);

/**
 * @brief Checks whether an ISO/IEC 14443 Type A card is present.
 *
 * Performs the card request operation used by the Identity Node polling loop.
 * A successful function return indicates that the communication procedure
 * completed; the actual detection result is returned through @p present.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param present Output pointer set to true when a compatible card responds,
 *                or false when no card is detected.
 *
 * @return
 *      - ESP_OK if the detection procedure completes successfully.
 *      - ESP_ERR_INVALID_ARG if the output pointer is invalid.
 *      - Another ESP-IDF error code if controller communication fails.
 */
esp_err_t ws1850s_card_present(i2c_master_dev_handle_t dev_handle, bool *present);

/**
 * @brief Reads the UID of a detected NFC card.
 *
 * Executes the card anti-collision sequence required to obtain the UID exposed
 * at cascade level one.
 *
 * The current implementation reads a four-byte UID through cascade level one.
 * The caller must provide an output buffer with capacity for at least four
 * bytes.
 *
 * @param dev_handle ESP-IDF I2C device handle associated with the WS1850S.
 * @param uid Output buffer with capacity for at least four bytes.
 * @param uid_len Output pointer that receives the UID length. On successful
 *                return, this value is set to four.
 *
 * @return
 *      - ESP_OK if a valid four-byte UID is acquired.
 *      - ESP_ERR_INVALID_ARG if @p uid or @p uid_len is NULL.
 *      - ESP_ERR_NOT_FOUND if no usable anti-collision response is received.
 *      - ESP_ERR_INVALID_CRC if the returned BCC does not match the UID bytes.
 *      - Another ESP-IDF error code if controller communication fails.
 */
esp_err_t ws1850s_read_uid(i2c_master_dev_handle_t dev_handle, uint8_t *uid, uint8_t *uid_len);

#ifdef __cplusplus
}
#endif

#endif

