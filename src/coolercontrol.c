#include "../include/coolercontrol.h"
#include "../include/config.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
    curl_mime_data(field, "100", CURL_ZERO_TERMINATED);
    
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
        // Search for NZXT Kraken device
        char *kraken_pos = strstr(response.data, "\"NZXT Kraken");
        if (kraken_pos) {
            // Extract name - search for end of name
            char *name_start = kraken_pos + 1;  // Skip opening quote
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
