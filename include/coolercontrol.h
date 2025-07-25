/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/**
 * @brief CoolerControl API interface for LCD device communication.
 * @details Provides functions for initializing, authenticating, and communicating with CoolerControl LCD devices.
 * @example
 *     See function documentation for usage examples.
 */

// Function prototypes
#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

// Include project headers
#include "config.h"

// Include necessary headers
#include <stddef.h>

/**
 * @brief Initializes a CoolerControl session and authenticates with the daemon using configuration.
 * @details Must be called before any other CoolerControl API function.
 * @example
 *     if (!init_coolercontrol_session(&config)) {
 *         // handle error
 *     }
 */
int init_coolercontrol_session(const Config *config);

/**
 * @brief Cleans up and terminates the CoolerControl session.
 * @details Frees all resources and closes the session.
 * @example
 *     cleanup_coolercontrol_session();
 */
void cleanup_coolercontrol_session(void);

/**
 * @brief Returns whether the session is initialized.
 * @details Checks if the session is ready for communication.
 * @example
 *     if (is_session_initialized()) {
 *         // session is ready
 *     }
 */
int is_session_initialized(void);

/**
 * @brief Retrieves the full name of the LCD device.
 * @details Gets the device name into the provided buffer.
 * @example
 *     char name[128];
 *     if (get_device_name(&config, name, sizeof(name))) {
 *         // use name
 *     }
 */
int get_device_name(const Config *config, char* name_buffer, size_t buffer_size);

/**
 * @brief Retrieves the UID of the first LCD device found.
 * @details Gets the device UID into the provided buffer.
 * @example
 *     char uid[128];
 *     if (get_device_uid(&config, uid, sizeof(uid))) {
 *         // use uid
 *     }
 */
int get_device_uid(const Config *config, char* uid_buffer, size_t buffer_size);

/**
 * @brief Initialize and cache the device UID at program start.
 * @details This function must be called once after session initialization.
 * @example
 *     if (!init_cached_device_uid(&config)) { ... }
 */
int init_cached_device_uid(const Config *config);

/**
 * @brief Get the cached device UID (read-only).
 * @details Returns a pointer to the cached UID string (empty string if not initialized).
 * @example
 *     const char* uid = get_cached_device_uid();
 *     if (uid[0]) { ... }
 */
const char* get_cached_device_uid(void);

/**
 * @brief Sends an image directly to the LCD of the CoolerControl device using configuration.
 * @details Uploads an image to the LCD display.
 * @example
 *     send_image_to_lcd(&config, "/opt/coolerdash/images/coolerdash.png", uuid);
 */
int send_image_to_lcd(const Config *config, const char* image_path, const char* device_uid);

/**
 * @brief Alias for send_image_to_lcd for API compatibility.
 * @details This function is provided for compatibility with other APIs and simply calls send_image_to_lcd().
 * @example
 *     upload_image_to_device(&config, "/opt/coolerdash/images/coolerdash.png", uuid);
 */
int upload_image_to_device(const Config *config, const char* image_path, const char* device_uid);

#endif // COOLERCONTROL_H
