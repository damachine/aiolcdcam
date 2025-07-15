#include "../include/coolercontrol.h"
#include "../include/config.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

// Response buffer structure for HTTP responses
struct http_response {
    char *data;
    size_t size;
};

// Callback function to write HTTP response data
static size_t write_callback(void *contents, size_t size, size_t nmemb, struct http_response *response) {
    size_t realsize = size * nmemb;
    char *ptr = realloc(response->data, response->size + realsize + 1);
    if (!ptr) return 0;  // Out of memory
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, realsize);
    response->size += realsize;
    response->data[response->size] = 0;  // Null-terminate
    
    return realsize;
}

// Global variables for HTTP session
static CURL *curl_handle = NULL;
static char cookie_jar[256] = {0};
static int session_initialized = 0;
static char cached_aio_uuid[128] = {0};  // Cache for detected AIO UUID

// UUID cache file path
#define UUID_CACHE_DIR "/var/cache/coolerdash"
#define UUID_CACHE_FILE "/var/cache/coolerdash/device.uuid"

/**
 * Initializes cURL and authenticates with the CoolerControl daemon
 *
 * @return 1 on success, 0 on error
 */
int init_coolercontrol_session(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT); 
    curl_handle = curl_easy_init();
    if (!curl_handle) return 0;
    
    // Cookie jar for session management
    snprintf(cookie_jar, sizeof(cookie_jar), "/tmp/nzxt_cookies_%d.txt", getpid());
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, cookie_jar);
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, cookie_jar);
    
    // Login to daemon
    char login_url[128];
    snprintf(login_url, sizeof(login_url), "%s/login", DAEMON_ADDRESS);
    
    char userpwd[64];
    snprintf(userpwd, sizeof(userpwd), "CCAdmin:%s", DAEMON_PASSWORD);
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, login_url);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, userpwd);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL); // Ignore response
    
    CURLcode res = curl_easy_perform(curl_handle);
    long response_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    if (res == CURLE_OK && (response_code == 200 || response_code == 204)) {
        session_initialized = 1;
        return 1;
    }
    
    return 0;
}

/**
 * Sends an image directly to the LCD of the CoolerControl device
 *
 * @param image_path Path to the image
 * @param device_uid Device UID
 * @return 1 on success, 0 on error
 */
int send_image_to_lcd(const char* image_path, const char* device_uid) {
    if (!curl_handle || !image_path || !device_uid || !session_initialized) return 0;
    
    // Check if file exists
    FILE *test_file = fopen(image_path, "rb");
    if (!test_file) return 0;
    fclose(test_file);
    
    // URL for LCD image upload
    char upload_url[256];
    snprintf(upload_url, sizeof(upload_url), 
             "%s/devices/%s/settings/lcd/lcd/images", DAEMON_ADDRESS, device_uid);
    
    // Determine MIME type
    const char* mime_type = "image/png";
    if (strstr(image_path, ".jpg") || strstr(image_path, ".jpeg")) {
        mime_type = "image/jpeg";
    } else if (strstr(image_path, ".gif")) {
        mime_type = "image/gif";
    }
    
    // Create multipart form
    curl_mime *form = curl_mime_init(curl_handle);
    curl_mimepart *field;
    
    // mode field
    field = curl_mime_addpart(form);
    curl_mime_name(field, "mode");
    curl_mime_data(field, "image", CURL_ZERO_TERMINATED);
    
    // brightness field
    field = curl_mime_addpart(form);
    curl_mime_name(field, "brightness");
    curl_mime_data(field, "50", CURL_ZERO_TERMINATED);
    
    // orientation field
    field = curl_mime_addpart(form);
    curl_mime_name(field, "orientation");
    curl_mime_data(field, "0", CURL_ZERO_TERMINATED);
    
    // images[] field (the actual image)
    field = curl_mime_addpart(form);
    curl_mime_name(field, "images[]");
    curl_mime_filedata(field, image_path);
    curl_mime_type(field, mime_type);
    
    // Configure cURL
    curl_easy_setopt(curl_handle, CURLOPT_URL, upload_url);
    curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, form);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
    
    // Execute request
    CURLcode res = curl_easy_perform(curl_handle);
    long response_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    // Cleanup
    curl_mime_free(form);
    curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    
    return (res == CURLE_OK && response_code == 200);
}

/**
 * Alias function for send_image_to_lcd (for better API compatibility)
 */
int upload_image_to_device(const char* image_path, const char* device_uid) {
    return send_image_to_lcd(image_path, device_uid);
}

/**
 * Terminates the CoolerControl session and cleans up
 */
void cleanup_coolercontrol_session(void) {
    static int cleanup_done = 0;
    if (cleanup_done) return;
    cleanup_done = 1;
    
    if (curl_handle) {
        curl_easy_cleanup(curl_handle);
        curl_handle = NULL;
    }
    curl_global_cleanup();
    unlink(cookie_jar); // Delete cookie file
    session_initialized = 0;
}

/**
 * Returns whether the session is initialized
 *
 * @return 1 if initialized, 0 if not
 */
int is_session_initialized(void) {
    return session_initialized;
}

/**
 * Retrieves the full name of the AIO LCD device
 *
 * @param name_buffer Buffer for the device name
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 */
int get_aio_device_name(char* name_buffer, size_t buffer_size) {
    if (!curl_handle || !name_buffer || buffer_size == 0 || !session_initialized) return 0;
    
    // Initialize response buffer
    struct http_response response = {0};
    response.data = malloc(1);  // Will be grown as needed by realloc
    response.size = 0;
    if (!response.data) return 0;
    
    // URL for device list
    char devices_url[128];
    snprintf(devices_url, sizeof(devices_url), "%s/devices", DAEMON_ADDRESS);
    
    // Configure cURL for GET request
    curl_easy_setopt(curl_handle, CURLOPT_URL, devices_url);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);  // GET request
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    
    // Execute request
    CURLcode res = curl_easy_perform(curl_handle);
    long response_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    // Reset cURL options
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
    
    int success = 0;
    if (res == CURLE_OK && response_code == 200 && response.data) {
        // Search for AIO device
        char *aio_pos = strstr(response.data, "\"NZXT Kraken");
        if (aio_pos) {
            // Extract name - search for end of name
            char *name_start = aio_pos + 1;  // Skip opening quote
            char *name_end = strchr(name_start, '"');
            if (name_end) {
                size_t name_length = name_end - name_start;
                if (name_length < buffer_size) {
                    strncpy(name_buffer, name_start, name_length);
                    name_buffer[name_length] = '\0';
                    success = 1;
                }
            }
        }
    }
    
    // Cleanup
    if (response.data) free(response.data);
    
    return success;
}

/**
 * Retrieves the UUID of the first AIO LCD device found
 *
 * @param uuid_buffer Buffer for the device UUID
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 */
int get_aio_device_uuid(char* uuid_buffer, size_t buffer_size) {
    if (!curl_handle || !uuid_buffer || buffer_size == 0 || !session_initialized) return 0;
    
    // Initialize response buffer
    struct http_response response = {0};
    response.data = malloc(1);  // Will be grown as needed by realloc
    response.size = 0;
    if (!response.data) return 0;
    
    // URL for device list
    char devices_url[128];
    snprintf(devices_url, sizeof(devices_url), "%s/devices", DAEMON_ADDRESS);
    
    // Configure cURL for GET request
    curl_easy_setopt(curl_handle, CURLOPT_URL, devices_url);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);  // GET request
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    
    // Execute request
    CURLcode res = curl_easy_perform(curl_handle);
    long response_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    // Reset cURL options
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
    
    int success = 0;
    if (res == CURLE_OK && response_code == 200 && response.data) {
        // Search for Liquidctl devices (AIO controllers)
        // Liquidctl devices are the most reliable indicator for AIO systems
        char *search_pos = response.data;
        char *uuid_start = NULL;
        
        while ((search_pos = strstr(search_pos, "\"uid\":\"")) != NULL) {
            // Found a UID, check if this device is an AIO with LCD
            uuid_start = search_pos + 7;  // Skip "uid":"
            char *uuid_end = strchr(uuid_start, '"');
            if (!uuid_end) {
                search_pos = uuid_start;
                continue;
            }
            
            // Extract the UUID
            size_t uuid_length = uuid_end - uuid_start;
            char temp_uuid[128];
            if (uuid_length >= sizeof(temp_uuid)) {
                search_pos = uuid_end;
                continue;
            }
            strncpy(temp_uuid, uuid_start, uuid_length);
            temp_uuid[uuid_length] = '\0';
            
            // Check if this device is a Liquidctl AIO device
            // Look for "type": "Liquidctl" which indicates an AIO controller
            char *device_start = search_pos;
            while (device_start > response.data && *device_start != '{') device_start--;
            char *device_end = strchr(search_pos, '}');
            if (!device_end) device_end = response.data + response.size;
            
            // Look for indicators that this is an AIO LCD device
            char device_section[4096];
            size_t section_length = device_end - device_start;
            if (section_length < sizeof(device_section)) {
                strncpy(device_section, device_start, section_length);
                device_section[section_length] = '\0';
                
                // Check for Liquidctl type - this is the most reliable indicator for AIO devices
                if (strstr(device_section, "\"type\":\"Liquidctl\"")) {
                    
                    // Found a Liquidctl device (AIO controller)
                    if (uuid_length < buffer_size) {
                        strncpy(uuid_buffer, temp_uuid, uuid_length);
                        uuid_buffer[uuid_length] = '\0';
                        
                        // Cache the UUID for future use
                        if (uuid_length < sizeof(cached_aio_uuid)) {
                            strcpy(cached_aio_uuid, uuid_buffer);
                            // Save to persistent cache
                            save_cached_uuid(uuid_buffer);
                        }
                        
                        success = 1;
                        break;
                    }
                }
            }
            
            search_pos = uuid_end;
        }
    }
    
    // Cleanup
    if (response.data) free(response.data);
    
    return success;
}

/**
 * Returns the cached AIO UUID (automatically detected at runtime)
 * This function will attempt to detect the UUID on first call if not cached
 *
 * @return Pointer to cached UUID string, or NULL on error
 */
const char* get_cached_aio_uuid(void) {
    // If already cached in memory, validate it first
    if (cached_aio_uuid[0] != '\0') {
        if (validate_cached_uuid(cached_aio_uuid)) {
            return cached_aio_uuid;
        } else {
            // Invalid, clear it
            cached_aio_uuid[0] = '\0';
        }
    }
    
    // Try to load from persistent cache
    if (load_cached_uuid()) {
        // Validate the loaded UUID
        if (validate_cached_uuid(cached_aio_uuid)) {
            return cached_aio_uuid;
        } else {
            // Invalid cached UUID, clear it
            clear_uuid_cache();
        }
    }
    
    // No valid cache found, detect UUID from API
    printf("No valid UUID cache found, detecting device...\n");
    if (get_aio_device_uuid(cached_aio_uuid, sizeof(cached_aio_uuid))) {
        // Save the newly detected UUID to cache
        save_cached_uuid(cached_aio_uuid);
        return cached_aio_uuid;
    }
    
    return NULL;  // Detection failed
}

/**
 * Loads cached UUID from persistent storage
 *
 * @return 1 on success, 0 if no valid cache found
 */
int load_cached_uuid(void) {
    FILE *cache_file = fopen(UUID_CACHE_FILE, "r");
    if (!cache_file) {
        return 0;  // No cache file found
    }
    
    char uuid_from_file[128] = {0};
    if (fgets(uuid_from_file, sizeof(uuid_from_file), cache_file)) {
        // Remove newline if present
        size_t len = strlen(uuid_from_file);
        if (len > 0 && uuid_from_file[len-1] == '\n') {
            uuid_from_file[len-1] = '\0';
        }
        
        // Validate UUID format (basic check)
        if (len > 10 && len < 120) {  // Reasonable UUID length
            strcpy(cached_aio_uuid, uuid_from_file);
            fclose(cache_file);
            printf("UUID loaded from cache: %s\n", cached_aio_uuid);
            return 1;
        }
    }
    
    fclose(cache_file);
    return 0;
}

/**
 * Saves UUID to persistent cache file
 *
 * @param uuid The UUID to cache
 * @return 1 on success, 0 on error
 */
int save_cached_uuid(const char* uuid) {
    if (!uuid || strlen(uuid) == 0) {
        return 0;
    }
    
    // Create cache directory if it doesn't exist
    struct stat st = {0};
    if (stat(UUID_CACHE_DIR, &st) == -1) {
        if (mkdir(UUID_CACHE_DIR, 0755) != 0) {
            return 0;  // Cannot create directory
        }
    }
    
    FILE *cache_file = fopen(UUID_CACHE_FILE, "w");
    if (!cache_file) {
        return 0;  // Cannot create cache file
    }
    
    if (fprintf(cache_file, "%s\n", uuid) > 0) {
        fclose(cache_file);
        printf("UUID saved to cache: %s\n", uuid);
        return 1;
    }
    
    fclose(cache_file);
    return 0;
}

/**
 * Validates if a cached UUID is still valid by testing it with CoolerControl
 *
 * @param uuid The UUID to validate
 * @return 1 if valid, 0 if invalid
 */
int validate_cached_uuid(const char* uuid) {
    if (!uuid || !curl_handle || !session_initialized) {
        return 0;
    }
    
    // Test UUID by checking if it exists in the devices list
    struct http_response response = {0};
    response.data = malloc(1);
    response.size = 0;
    if (!response.data) return 0;
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, DAEMON_ADDRESS "/devices");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl_handle);
    long response_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    // Reset cURL options
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
    
    int is_valid = 0;
    if (res == CURLE_OK && response_code == 200 && response.data) {
        // Check if UUID exists in the response
        if (strstr(response.data, uuid) != NULL) {
            is_valid = 1;
        }
    }
    
    if (response.data) free(response.data);
    
    if (is_valid) {
        printf("Cached UUID validated successfully: %s\n", uuid);
    } else {
        printf("Cached UUID is invalid, will re-detect: %s\n", uuid);
    }
    
    return is_valid;
}

/**
 * Clears the UUID cache (both memory and file)
 */
void clear_uuid_cache(void) {
    cached_aio_uuid[0] = '\0';
    unlink(UUID_CACHE_FILE);
    printf("UUID cache cleared\n");
}
