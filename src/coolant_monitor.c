/**
 * @file coolant_monitor.c
 * @brief Coolant temperature monitoring implementation for CoolerDash.
 *
 * @details
 * Implements functions for reading coolant temperature from system sensors and handling sensor path detection.
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
 * @see coolant_monitor.h, config.h
 *
 * @todo
 * - Add support for additional coolant sensor types if required.
 *
 * @example
 * See function documentation for usage examples.
 */

// Include project headers
#include "../include/coolant_monitor.h"
#include "../include/config.h"

// Include necessary headers
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

// Global variable for cached coolant temperature path
char coolant_temp_path[512] = {0}; // Holds the detected sensor file path for coolant temperature

/**
 * @brief Initializes the hwmon sensor path for coolant temperature at startup (once).
 *
 * Detects and sets the path to the coolant temperature sensor file by scanning available hwmon entries.
 * Uses the hwmon_path from the configuration struct.
 *
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * init_coolant_sensor_path(&config);
 * @endcode
 */
void init_coolant_sensor_path(const Config *config) {
    DIR *dir = opendir(config->hwmon_path);
    if (!dir) return;
    
    struct dirent *entry;
    char label_path[512], label[64];
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        for (int i = 1; i <= 9; ++i) {
            snprintf(label_path, sizeof(label_path), "%s/%s/temp%d_label", config->hwmon_path, entry->d_name, i);
            FILE *flabel = fopen(label_path, "r");
            if (!flabel) continue;
            
            if (fgets(label, sizeof(label), flabel)) {
                // Cache coolant temperature path
                if ((strstr(label, "Coolant") || strstr(label, "coolant")) && strlen(coolant_temp_path) == 0) {
                    snprintf(coolant_temp_path, sizeof(coolant_temp_path), "%s/%s/temp%d_input", config->hwmon_path, entry->d_name, i);
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
 * @brief Reads coolant temperature from cached hwmon path.
 *
 * Reads the temperature from the coolant sensor file set by init_coolant_sensor_path().
 *
 * @return Temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_coolant_temp();
 * @endcode
 */
float read_coolant_temp(void) {
    if (strlen(coolant_temp_path) == 0) return 0.0f;
    
    FILE *finput = fopen(coolant_temp_path, "r");
    if (!finput) return 0.0f;
    
    int t = 0;
    float temp = 0.0f;
    if (fscanf(finput, "%d", &t) == 1) {
        temp = t > 200 ? t / 1000.0f : (float)t;
    }
    fclose(finput);
    return temp;
}
