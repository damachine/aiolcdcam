/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file gpu_monitor.c
 * @brief GPU temperature monitoring implementation for CoolerDash.
 *
 * @details
 * Implements functions for reading GPU temperature from system sensors and handling GPU availability.
 *
 * Coding and Documentation Standards for CoolerDash:
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
 * C99 Coding Guidelines:
 * - Follow ISO/IEC 9899:1999 (C99)
 * - Check return values of malloc(), calloc(), realloc()
 * - Free dynamic memory and set pointers to NULL
 * - Use include guards: #ifndef HEADER_H / #define HEADER_H / #endif
 * - Only include necessary headers; separate system and local headers
 * - Use 4 spaces for indentation, no tabs
 * - Use const for immutable variables and parameters
 * - Use static for file-local functions/variables
 * - Use inline for small, frequently used functions
 *
 * Naming Conventions:
 * - Functions: snake_case verbs (e.g. calculate_sum())
 * - Variables: snake_case (e.g. user_count)
 * - Constants/Macros: UPPER_CASE (e.g. MAX_SIZE)
 * - Structs via typedef: PascalCase (e.g. MyStruct)
 * - Use descriptive names, avoid abbreviations
 * - Use enum for status/error codes
 * - Use typedef for complex types
 * - Consistent naming throughout the project
 *
 * @warning This file must comply with ISO/IEC 9899:1999 (C99).
 * @see gpu_monitor.h, config.h
 * @todo Add support for additional GPU sensor types if required.
 * @example See function documentation for usage examples.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

// Include project headers
#include "../include/gpu_monitor.h"
#include "../include/config.h"
// gpu_monitor.h provides all function prototypes and the gpu_data_t struct for GPU monitoring.
// config.h is required for access to the Config struct and macro defaults.

// Include necessary headers
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// Global variable for GPU availability
static int gpu_available = -1;  // -1 = unknown, 0 = not available, 1 = available

/**
 * @brief Get current time in milliseconds.
 *
 * Returns the current system time in milliseconds since the epoch.
 *
 * @return Current time in milliseconds
 *
 * Example:
 * @code
 * long long now = get_current_time_ms();
 * @endcode
 */
static long long get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (long long)(tv.tv_usec) / 1000;
}

/**
 * @brief Checks GPU availability and initializes GPU monitoring using configuration.
 *
 * Checks if an NVIDIA GPU is available and initializes the monitoring backend.
 * Uses config values for cache interval etc.
 *
 * @param config Pointer to configuration struct (Config)
 * @return 1 if GPU available, 0 if not
 *
 * Example:
 * @code
 * if (init_gpu_monitor(&config)) {
 *     // GPU available
 * }
 * @endcode
 */
int init_gpu_monitor(const Config *config) {
    (void)config; // suppress unused parameter warning (C99)
    
    if (gpu_available != -1) {
        return gpu_available;  // Already checked
    }
    
    FILE *fp = popen("nvidia-smi -L 2>/dev/null", "r");
    if (fp) {
        char line[256];
        if (fgets(line, sizeof(line), fp) != NULL) {
            gpu_available = 1;  // GPU found
        } else {
            gpu_available = 0;  // No GPU found
        }
        pclose(fp);
    } else {
        gpu_available = 0;  // nvidia-smi not available
    }
    
    return gpu_available;
}

/**
 * @brief Reads only GPU temperature (optimized for mode "def").
 *
 * Reads the current temperature from the GPU sensor, with caching for performance.
 *
 * @param config Pointer to configuration struct (Config)
 * @return GPU temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_gpu_temp();
 * @endcode
 */
float read_gpu_temp(const Config *config) {
    if (!init_gpu_monitor(config)) return 0.0f;  // GPU not available
    
    static long long last_update_ms = 0;
    static float cached_temp = 0;
    long long now_ms = get_current_time_ms();
    long long cache_interval_ms = (long long)(config->gpu_cache_interval * 1000);
    
    if (now_ms - last_update_ms >= cache_interval_ms) {
        FILE *fp = popen("nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            if (fscanf(fp, "%f", &cached_temp) != 1) {
                cached_temp = 0.0f;
            }
            pclose(fp);
            last_update_ms = now_ms;
        }
    }
    return cached_temp;
}

/**
 * @brief Reads GPU temperature only (usage and memory usage always 0).
 *
 * Fills a gpu_data_t structure with temperature value, usage and memory usage set to 0.
 *
 * @param config Pointer to configuration struct (Config)
 * @param[out] data Pointer to GPU data structure
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * gpu_data_t data;
 * if (get_gpu_data_full(&config, &data)) {
 *     // use data
 * }
 * @endcode
 */
int get_gpu_data_full(const Config *config, gpu_data_t *data) {
    if (!data) return 0;
    data->temperature = read_gpu_temp(config);
    return 1;
}
