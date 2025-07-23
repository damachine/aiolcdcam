/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file cpu_monitor.c
 * @brief CPU temperature monitoring implementation for CoolerDash.
 *
 * @details
 * Implements functions for reading CPU temperature from system sensors.
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
 * @see cpu_monitor.h, config.h
 * @todo Add support for additional sensor types if required.
 * @example See function documentation for usage examples.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

// Include project headers
#include "../include/cpu_monitor.h"
#include "../include/config.h"
// The config.h header is required for access to the Config struct and macro defaults.
// cpu_monitor.h provides all function prototypes and the cpu_stat_t struct for usage calculations.

// Include necessary headers
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

// Global variable for cached CPU temperature path
char cpu_temp_path[512] = {0};

/**
 * @brief Initialize hwmon sensor path for CPU temperature at startup (once).
 *
 * Detects and sets the path to the CPU temperature sensor file by scanning available hwmon entries.
 * Uses the hwmon_path from the configuration struct.
 *
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * init_cpu_sensor_path(&config);
 * @endcode
 */
void init_cpu_sensor_path(const Config *config) {
    DIR *dir = opendir(config->hwmon_path);
    if (!dir) return;
    
    struct dirent *entry;
    char label_path[512], label[64];
    
    // Scan through hwmon directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files/directories
        
        // Check each possible temp label for the current hwmon entry
        for (int i = 1; i <= 9; ++i) {
            snprintf(label_path, sizeof(label_path), "%s/%s/temp%d_label", config->hwmon_path, entry->d_name, i);
            FILE *flabel = fopen(label_path, "r");
            if (!flabel) continue;
            
            // Read the label and check if it matches "Package id 0"
            if (fgets(label, sizeof(label), flabel)) {
                // Cache CPU temperature path for later use
                if (strstr(label, "Package id 0") && strlen(cpu_temp_path) == 0) {
                    snprintf(cpu_temp_path, sizeof(cpu_temp_path), "%s/%s/temp%d_input", config->hwmon_path, entry->d_name, i);
                    fclose(flabel);
                    closedir(dir);
                    return;
                }
            }
            fclose(flabel);
        }
    }
    closedir(dir);
}

/**
 * @brief Read CPU temperature from cached hwmon path.
 *
 * Reads the temperature from the CPU sensor file set by init_cpu_sensor_path().
 *
 * @return Temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_cpu_temp();
 * @endcode
 */
float read_cpu_temp(void) {
    if (strlen(cpu_temp_path) == 0) return 0.0f;
    
    FILE *finput = fopen(cpu_temp_path, "r");
    if (!finput) return 0.0f;
    
    int t = 0;
    float temp = 0.0f;
    // Read the integer temperature value, convert to float
    if (fscanf(finput, "%d", &t) == 1) {
        temp = t > 200 ? t / 1000.0f : (float)t;
    }
    fclose(finput);
    return temp;
}

/**
 * @brief Read RAM usage from /proc/meminfo.
 *
 * @details
 * Reads and parses /proc/meminfo to calculate RAM usage as a percentage. This function is deprecated and kept for legacy reasons only.
 * RAM usage is no longer used in CoolerDash and will be removed in future versions.
 *
 * @return RAM usage in percent (float), -1.0f on error
 * @note This function is kept for legacy reasons only.
 * @warning If /proc/meminfo format changes, this function may return incorrect results.
 * @deprecated RAM usage is no longer used in CoolerDash. This function will be removed in future versions.
 * @see cpu_monitor.h
 * @author damachine
 * @since 0.25.07.23.2
 * @version 0.25.07.23.2
 * @example
 *     float ram = get_ram_usage();
 */
float get_ram_usage(void) {
    FILE *fmem = fopen("/proc/meminfo", "r");
    if (!fmem) return -1.0f;
    
    long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    char line[128];
    int found = 0;
    
    // Read and parse relevant lines from /proc/meminfo
    while (fgets(line, sizeof(line), fmem) && found < 4) {
        if (sscanf(line, "MemTotal: %ld", &mem_total) == 1) found++;
        else if (sscanf(line, "MemFree: %ld", &mem_free) == 1) found++;
        else if (sscanf(line, "Buffers: %ld", &buffers) == 1) found++;
        else if (sscanf(line, "Cached: %ld", &cached) == 1) found++;
    }
    fclose(fmem);
    
    if (found == 4 && mem_total > 0) {
        const long mem_used = mem_total - (mem_free + buffers + cached);
        if (mem_used >= 0) {
            return 100.0f * (float)mem_used / (float)mem_total;
        }
    }
    return -1.0f;
}
