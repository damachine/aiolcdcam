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
int get_kraken_device_name(char* name_buffer, size_t buffer_size);

// Session Status
int is_session_initialized(void);

#endif // COOLERCONTROL_H
