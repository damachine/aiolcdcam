#ifndef COOLERCONTROL_H
#define COOLERCONTROL_H

#include <stddef.h>  // For size_t

// =============================================================================
// COOLERCONTROL API MODULE
// =============================================================================

// CoolerControl Session Functions
int init_coolercontrol_session(void);
int send_image_to_lcd(const char* image_path, const char* device_uid);
int upload_image_to_device(const char* image_path, const char* device_uid);
void cleanup_coolercontrol_session(void);

// Device Information Functions
int get_aio_device_name(char* name_buffer, size_t buffer_size);
int get_aio_device_uuid(char* uuid_buffer, size_t buffer_size);
const char* get_cached_aio_uuid(void);

// UUID Caching Functions
int load_cached_uuid(void);
int save_cached_uuid(const char* uuid);
int validate_cached_uuid(const char* uuid);
void clear_uuid_cache(void);

// Session Status
int is_session_initialized(void);

#endif // COOLERCONTROL_H
