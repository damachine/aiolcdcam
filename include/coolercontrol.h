/**
 * @file coolercontrol.h
 * @brief CoolerControl API interface for LCD device communication.
 *
 * Coding Standards (C99, ISO/IEC 9899:1999):
 * - All code comments in English.
 * - Doxygen-style comments for all functions (description, @brief, @param, @return, examples).
 * - Opening braces on the same line (K&R style).
 * - Always check return values of malloc(), calloc(), realloc().
 * - Free all dynamically allocated memory and set pointers to NULL after freeing.
 * - Use include guards in all headers.
 * - Include only necessary headers, system headers before local headers.
 * - Function names are verbs, use snake_case for functions/variables, UPPER_CASE for macros, PascalCase for typedef structs.
 * - Use descriptive names, avoid abbreviations.
 * - Document complex algorithms and data structures.
 */
#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

#include <stddef.h>  // For size_t

// =============================================================================
// COOLERCONTROL API MODULE
// =============================================================================

// --- Session Management ---
/**
 * @brief Initializes a CoolerControl session and authenticates with the daemon.
 *
 * Must be called before any other CoolerControl API function.
 *
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * if (!init_coolercontrol_session()) {
 *     // handle error
 * }
 * @endcode
 */
int init_coolercontrol_session(void);

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
int get_device_name(char* name_buffer, size_t buffer_size);

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
int get_device_uid(char* uid_buffer, size_t buffer_size);

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
int init_cached_device_uid(void);

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
 * @brief Sends an image directly to the LCD of the CoolerControl device.
 *
 * Uploads an image to the LCD display.
 *
 * @param image_path Path to the image file (PNG/JPEG/GIF)
 * @param device_uid Device UID string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * send_image_to_lcd("/opt/coolerdash/images/coolerdash.png", uuid);
 * @endcode
 */
int send_image_to_lcd(const char* image_path, const char* device_uid);

/**
 * @brief Alias for send_image_to_lcd for API compatibility.
 *
 * @param image_path Path to the image file
 * @param device_uid Device UID string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * upload_image_to_device("/opt/coolerdash/images/coolerdash.png", uuid);
 * @endcode
 */
int upload_image_to_device(const char* image_path, const char* device_uid);

#endif // COOLERCONTROL_H
