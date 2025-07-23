/**
 * @file config.c
 * @brief Central configuration implementation for CoolerDash.
 *
 * @details
 * Coding and Documentation Standards for CoolerDash:
 * - All code comments must be in English and use Doxygen style for functions.
 * - Use @brief, @details, @param, @return, @throws, @pre, @post, @note, @warning, @bug, @todo, @see, @example, @deprecated, @since, @version, @author, @copyright, @license
 * - Opening braces for functions and control structures on the same line (K&R style)
 * - Comment all non-obvious code sections
 * - Avoid redundant comments
 * - Document complex algorithms and data structures thoroughly
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
 * @note All configuration values are loaded exclusively from the INI file at runtime.
 * @copyright (c) 2025 damachine
 * @license MIT
 */
#include <ini.h>           // External INI parser
#include "../include/config.h" // Project config header
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief INI parser handler, sets values in Config struct.
 *
 * Called for each key-value pair in the INI file. Matches section and key names and sets the corresponding field in the Config struct.
 *
 * @param user Pointer to Config struct
 * @param section Current section name
 * @param name Current key name
 * @param value Current value as string
 * @return 1 on success, 0 on error
 *
 * @code
 * ini_parse("/etc/coolerdash/config.ini", inih_config_handler, &cfg);
 * @endcode
 */
static int inih_config_handler(void *user, const char *section, const char *name, const char *value)
{
    Config *config = (Config *)user;

    // Display section
    if (strcmp(section, "display") == 0) {
        if (strcmp(name, "width") == 0) config->display_width = atoi(value);
        else if (strcmp(name, "height") == 0) config->display_height = atoi(value);
        else if (strcmp(name, "refresh_interval_sec") == 0) config->display_refresh_interval_sec = atoi(value);
        else if (strcmp(name, "refresh_interval_nsec") == 0) config->display_refresh_interval_nsec = atoi(value);
        else if (strcmp(name, "brightness") == 0) config->lcd_brightness = atoi(value);
        else if (strcmp(name, "orientation") == 0) config->lcd_orientation = atoi(value);
    }
    // Layout section
    else if (strcmp(section, "layout") == 0) {
        if (strcmp(name, "box_width") == 0) config->box_width = atoi(value);
        else if (strcmp(name, "box_height") == 0) config->box_height = atoi(value);
        else if (strcmp(name, "box_gap") == 0) config->box_gap = atoi(value);
        else if (strcmp(name, "bar_width") == 0) config->bar_width = atoi(value);
        else if (strcmp(name, "bar_height") == 0) config->bar_height = atoi(value);
        else if (strcmp(name, "bar_gap") == 0) config->bar_gap = atoi(value);
        else if (strcmp(name, "border_line_width") == 0) config->border_line_width = (float)atof(value);
    }
    // Font section
    else if (strcmp(section, "font") == 0) {
        if (strcmp(name, "face") == 0) strncpy(config->font_face, value, sizeof(config->font_face) - 1);
        else if (strcmp(name, "size_large") == 0) config->font_size_large = (float)atof(value);
        else if (strcmp(name, "size_labels") == 0) config->font_size_labels = (float)atof(value);
    }
    // Temperature section
    else if (strcmp(section, "temperature") == 0) {
        if (strcmp(name, "threshold_green") == 0) config->temp_threshold_green = (float)atof(value);
        else if (strcmp(name, "threshold_orange") == 0) config->temp_threshold_orange = (float)atof(value);
        else if (strcmp(name, "threshold_red") == 0) config->temp_threshold_red = (float)atof(value);
    }
    // Cache section
    else if (strcmp(section, "cache") == 0) {
        if (strcmp(name, "gpu_interval") == 0) config->gpu_cache_interval = (float)atof(value);
        else if (strcmp(name, "change_tolerance_temp") == 0) config->change_tolerance_temp = (float)atof(value);
    }
    // Paths section
    else if (strcmp(section, "paths") == 0) {
        if (strcmp(name, "hwmon") == 0) strncpy(config->hwmon_path, value, sizeof(config->hwmon_path) - 1);
        else if (strcmp(name, "image_dir") == 0) strncpy(config->image_dir, value, sizeof(config->image_dir) - 1);
        else if (strcmp(name, "image_path") == 0) strncpy(config->image_path, value, sizeof(config->image_path) - 1);
        else if (strcmp(name, "shutdown_image") == 0) strncpy(config->shutdown_image, value, sizeof(config->shutdown_image) - 1);
        else if (strcmp(name, "pid_file") == 0) strncpy(config->pid_file, value, sizeof(config->pid_file) - 1);
    }
    // Daemon section
    else if (strcmp(section, "daemon") == 0) {
        if (strcmp(name, "address") == 0) strncpy(config->daemon_address, value, sizeof(config->daemon_address) - 1);
        else if (strcmp(name, "password") == 0) strncpy(config->daemon_password, value, sizeof(config->daemon_password) - 1);
    }
    // Color sections
    else if (strcmp(section, "color_green") == 0) {
        if (strcmp(name, "r") == 0) config->color_green.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_green.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_green.b = atoi(value);
    }
    else if (strcmp(section, "color_orange") == 0) {
        if (strcmp(name, "r") == 0) config->color_orange.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_orange.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_orange.b = atoi(value);
    }
    else if (strcmp(section, "color_hot_orange") == 0) {
        if (strcmp(name, "r") == 0) config->color_hot_orange.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_hot_orange.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_hot_orange.b = atoi(value);
    }
    else if (strcmp(section, "color_red") == 0) {
        if (strcmp(name, "r") == 0) config->color_red.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_red.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_red.b = atoi(value);
    }
    else if (strcmp(section, "color_temp") == 0) {
        if (strcmp(name, "r") == 0) config->color_temp.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_temp.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_temp.b = atoi(value);
    }
    else if (strcmp(section, "color_label") == 0) {
        if (strcmp(name, "r") == 0) config->color_label.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_label.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_label.b = atoi(value);
    }
    else if (strcmp(section, "color_bg") == 0) {
        if (strcmp(name, "r") == 0) config->color_bg.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_bg.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_bg.b = atoi(value);
    }
    else if (strcmp(section, "color_border") == 0) {
        if (strcmp(name, "r") == 0) config->color_border.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_border.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_border.b = atoi(value);
    }
    return 1; // success
}

/**
 * @brief Loads configuration from INI file.
 *
 * Parses the INI file and fills the Config struct.
 * If the file cannot be read, returns error.
 *
 * @param config Pointer to Config struct to fill
 * @param path Path to INI file
 * @return 0 on success, -1 on error
 *
 * @code
 * Config cfg;
 * if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *     // handle error
 * }
 * @endcode
 */
int load_config_ini(Config *config, const char *path)
{
    if (!config || !path) return -1;
    int error = ini_parse(path, inih_config_handler, config);
    if (error < 0) {
        // File not found or parse error
        return -1;
    }
    return 0;
}