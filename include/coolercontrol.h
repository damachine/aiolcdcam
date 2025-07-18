#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

#include <stddef.h>  // For size_t

// =============================================================================
// COOLERCONTROL API MODULE
// =============================================================================

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
 * @brief Sends an image directly to the LCD of the CoolerControl device.
 *
 * Uploads an image to the AIO LCD display.
 *
 * @param image_path Path to the image file (PNG/JPEG/GIF)
 * @param device_uid Device UUID string
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
 * @param device_uid Device UUID string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * upload_image_to_device("/opt/coolerdash/images/coolerdash.png", uuid);
 * @endcode
 */
int upload_image_to_device(const char* image_path, const char* device_uid);

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

// Device Information Functions
/**
 * @brief Retrieves the full name of the AIO LCD device.
 *
 * @param[out] name_buffer Buffer to store the device name
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * char name[128];
 * if (get_aio_device_name(name, sizeof(name))) {
 *     // use name
 * }
 * @endcode
 */
int get_aio_device_name(char* name_buffer, size_t buffer_size);

/**
 * @brief Retrieves the UUID of the first AIO LCD device found.
 *
 * @param[out] uuid_buffer Buffer to store the device UUID
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * char uuid[128];
 * if (get_aio_device_uuid(uuid, sizeof(uuid))) {
 *     // use uuid
 * }
 * @endcode
 */
int get_aio_device_uuid(char* uuid_buffer, size_t buffer_size);

/**
 * @brief Returns the cached AIO UUID (detected at runtime).
 *
 * @return Pointer to cached UUID string, or NULL on error
 *
 * Example:
 * @code
 * const char* uuid = get_cached_aio_uuid();
 * if (uuid) {
 *     // use uuid
 * }
 * @endcode
 */
const char* get_cached_aio_uuid(void);

// UUID Caching Functions
/**
 * @brief Loads cached UUID from persistent storage.
 *
 * @return 1 on success, 0 if no valid cache found
 *
 * Example:
 * @code
 * if (!load_cached_uuid()) {
 *     // no valid cache
 * }
 * @endcode
 */
int load_cached_uuid(void);

/**
 * @brief Saves UUID to persistent cache file.
 *
 * @param uuid The UUID to cache
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * save_cached_uuid(uuid);
 * @endcode
 */
int save_cached_uuid(const char* uuid);

/**
 * @brief Validates if a cached UUID is still valid by testing it with CoolerControl.
 *
 * @param uuid The UUID to validate
 * @return 1 if valid, 0 if invalid
 *
 * Example:
 * @code
 * if (!validate_cached_uuid(uuid)) {
 *     // uuid is invalid
 * }
 * @endcode
 */
int validate_cached_uuid(const char* uuid);

/**
 * @brief Clears the UUID cache (both memory and file).
 *
 * @return void
 *
 * Example:
 * @code
 * clear_uuid_cache();
 * @endcode
 */
void clear_uuid_cache(void);

// Session Status
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

#endif // COOLERCONTROL_H
