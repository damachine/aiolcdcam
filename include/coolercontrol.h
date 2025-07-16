#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

#include <stddef.h>  // For size_t

// =============================================================================
// COOLERCONTROL API MODULE
// =============================================================================

// CoolerControl Session Functions
/**
 * Initializes a CoolerControl session and authenticates with the daemon.
 * @return 1 on success, 0 on error
 * @brief Must be called before any other CoolerControl API function.
 */
int init_coolercontrol_session(void);

/**
 * Sends an image directly to the LCD of the CoolerControl device.
 * @param image_path Path to the image file (PNG/JPEG/GIF)
 * @param device_uid Device UUID string
 * @return 1 on success, 0 on error
 * @brief Uploads an image to the AIO LCD display.
 */
int send_image_to_lcd(const char* image_path, const char* device_uid);

/**
 * Alias for send_image_to_lcd for API compatibility.
 * @param image_path Path to the image file
 * @param device_uid Device UUID string
 * @return 1 on success, 0 on error
 */
int upload_image_to_device(const char* image_path, const char* device_uid);

/**
 * Cleans up and terminates the CoolerControl session.
 * @brief Frees all resources and closes the session.
 */
void cleanup_coolercontrol_session(void);

// Device Information Functions
/**
 * Retrieves the full name of the AIO LCD device.
 * @param name_buffer Buffer to store the device name
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 */
int get_aio_device_name(char* name_buffer, size_t buffer_size);

/**
 * Retrieves the UUID of the first AIO LCD device found.
 * @param uuid_buffer Buffer to store the device UUID
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 */
int get_aio_device_uuid(char* uuid_buffer, size_t buffer_size);

/**
 * Returns the cached AIO UUID (detected at runtime).
 * @return Pointer to cached UUID string, or NULL on error
 */
const char* get_cached_aio_uuid(void);

// UUID Caching Functions
/**
 * Loads cached UUID from persistent storage.
 * @return 1 on success, 0 if no valid cache found
 */
int load_cached_uuid(void);

/**
 * Saves UUID to persistent cache file.
 * @param uuid The UUID to cache
 * @return 1 on success, 0 on error
 */
int save_cached_uuid(const char* uuid);

/**
 * Validates if a cached UUID is still valid by testing it with CoolerControl.
 * @param uuid The UUID to validate
 * @return 1 if valid, 0 if invalid
 */
int validate_cached_uuid(const char* uuid);

/**
 * Clears the UUID cache (both memory and file).
 */
void clear_uuid_cache(void);

// Session Status
/**
 * Returns whether the session is initialized.
 * @return 1 if initialized, 0 if not
 */
int is_session_initialized(void);

#endif // COOLERCONTROL_H
