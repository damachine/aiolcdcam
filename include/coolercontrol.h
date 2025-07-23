/**
 * @file coolercontrol.h
 * @brief CoolerControl API interface for LCD device communication.
 *
 * @details
 * Provides functions for initializing, authenticating, and communicating with CoolerControl LCD devices.
 *
 * @author damachine
 * @copyright Copyright (c) 2025 damachine
 * @license MIT
 * @version 0.25.07.23.2
 * @since 0.25.07.23.2
 *
 * @note
 * - All code comments are written in English.
 * - Doxygen style is used for all function comments.
 * - See coding standards in project documentation and config.h for details.
 * - Opening braces for functions and control structures are placed on the same line (K&R style).
 * - Only necessary headers are included; system and local headers are separated.
 * - Code is indented with 4 spaces, no tabs.
 * - All functions, variables, and types follow project naming conventions (snake_case, PascalCase, UPPER_CASE).
 * - Inline comments are used sparingly and only when necessary.
 * - Redundant comments are avoided.
 * - All dynamically allocated memory is freed and pointers set to NULL.
 * - All malloc/calloc/realloc return values are checked.
 *
 * @warning
 * - This file must comply with ISO/IEC 9899:1999 (C99).
 * - Do not add obsolete or unused code.
 *
 * @see config.h
 *
 * @todo
 * - Add support for additional LCD device types if required.
 *
 * @example
 * See function documentation for usage examples.
 */
#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

#include <stddef.h>  // For size_t
#include "config.h"

/**
 * @brief Initializes a CoolerControl session and authenticates with the daemon using configuration.
 *
 * Must be called before any other CoolerControl API function.
 *
 * @param config Pointer to configuration struct (Config).
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * if (!init_coolercontrol_session(&config)) {
 *     // handle error
 * }
 * @endcode
 */
int init_coolercontrol_session(const Config *config);

/**
 * @brief Cleans up and terminates the CoolerControl session.
 *
 * Frees all resources and closes the session.
 *
 * @return void
 *
 * Example:
 * @code
 * cleanup_coolercontrol_session();
 * @endcode
 */
void cleanup_coolercontrol_session(void);

/**
 * @brief Returns whether the session is initialized.
 *
 * @return 1 if initialized, 0 if not
 *
 * Example:
 * @code
 * if (is_session_initialized()) {
 *     // session is ready
 * }
 * @endcode
 */
int is_session_initialized(void);

// --- Device Information ---
/**
 * @brief Retrieves the full name of the LCD device.
 *
 * @param[out] name_buffer Buffer to store the device name
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * char name[128];
 * if (get_device_name(name, sizeof(name))) {
 *     // use name
 * }
 * @endcode
 */
int get_device_name(const Config *config, char* name_buffer, size_t buffer_size);

/**
 * @brief Retrieves the UID of the first LCD device found.
 *
 * @param[out] uid_buffer Buffer to store the device UID
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * char uid[128];
 * if (get_device_uid(uid, sizeof(uid))) {
 *     // use uid
 * }
 * @endcode
 */
int get_device_uid(const Config *config, char* uid_buffer, size_t buffer_size);

/**
 * @brief Initialize and cache the device UID at program start.
 *
 * This function must be called once after session initialization.
 *
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * if (!init_cached_device_uid()) { ... }
 * @endcode
 */
int init_cached_device_uid(const Config *config);

/**
 * @brief Get the cached device UID (read-only).
 *
 * @return Pointer to cached UID string (empty string if not initialized)
 *
 * Example:
 * @code
 * const char* uid = get_cached_device_uid();
 * if (uid[0]) { ... }
 * @endcode
 */
const char* get_cached_device_uid(void);

// --- Image Upload ---
/**
 * @brief Sends an image directly to the LCD of the CoolerControl device using configuration.
 *
 * Uploads an image to the LCD display.
 *
 * @param config Pointer to configuration struct (Config).
 * @param image_path Path to the image file (PNG/JPEG/GIF)
 * @param device_uid Device UID string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * send_image_to_lcd(&config, "/opt/coolerdash/images/coolerdash.png", uuid);
 * @endcode
 */
int send_image_to_lcd(const Config *config, const char* image_path, const char* device_uid);

/**
 * @brief Alias for send_image_to_lcd for API compatibility.
 *
 * This function is provided for compatibility with other APIs and simply calls send_image_to_lcd().
 *
 * @param config Pointer to configuration struct (Config).
 * @param image_path Path to the image file
 * @param device_uid Device UID string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * upload_image_to_device(&config, "/opt/coolerdash/images/coolerdash.png", uuid);
 * @endcode
 */
int upload_image_to_device(const Config *config, const char* image_path, const char* device_uid);

#endif // COOLERCONTROL_H
