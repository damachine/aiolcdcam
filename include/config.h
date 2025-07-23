/**
 * @file config.h
 * @brief Central configuration header for CoolerDash.
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
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <ini.h>

/**
 * @brief Structure for runtime configuration loaded from INI file.
 *
 * All fields are loaded from the INI file. No static defaults are defined here.
 *
 * @details
 * - All display, layout, font, temperature, cache, path and color settings are loaded at runtime.
 * - Color fields use int (0-255) for RGB values.
 * - All string fields are zero-terminated and sized for safe usage.
 */
typedef struct Config {
    int display_width;           /**< Display width in pixels */
    int display_height;          /**< Display height in pixels */
    int display_refresh_interval_sec; /**< Display refresh interval (seconds) */
    int display_refresh_interval_nsec; /**< Display refresh interval (nanoseconds) */
    int lcd_brightness;          /**< LCD brightness (0-100) */
    int lcd_orientation;         /**< LCD orientation: 0, 90, 180, 270 */
    int box_width;               /**< Box width in pixels */
    int box_height;              /**< Box height in pixels */
    int box_gap;                 /**< Gap between boxes in pixels */
    int bar_width;               /**< Bar width in pixels */
    int bar_height;              /**< Bar height in pixels */
    int bar_gap;                 /**< Gap between bars in pixels */
    float border_line_width;     /**< Border line width in pixels */
    char font_face[64];          /**< Font face for display text */
    float font_size_large;       /**< Large font size */
    float font_size_labels;      /**< Label font size */
    float temp_threshold_green;  /**< Green threshold (°C) */
    float temp_threshold_orange; /**< Orange threshold (°C) */
    float temp_threshold_red;    /**< Red threshold (°C) */
    float gpu_cache_interval;    /**< GPU cache interval (seconds) */
    float change_tolerance_temp; /**< Temperature change tolerance (°C) */
    char hwmon_path[128];        /**< Path to hwmon */
    char image_dir[128];         /**< Directory for images */
    char image_path[128];        /**< Path for display image */
    char shutdown_image[128];    /**< Path for shutdown image */
    char pid_file[128];          /**< Path for PID file */
    char daemon_address[128];    /**< Daemon address */
    char daemon_password[64];    /**< Daemon password */
    struct { int r, g, b; } color_green;        /**< RGB for green bar */
    struct { int r, g, b; } color_orange;       /**< RGB for orange bar */
    struct { int r, g, b; } color_hot_orange;   /**< RGB for hot orange bar */
    struct { int r, g, b; } color_red;          /**< RGB for red bar */
    struct { int r, g, b; } color_temp;         /**< RGB for temperature text */
    struct { int r, g, b; } color_label;        /**< RGB for label text */
    struct { int r, g, b; } color_bg;           /**< RGB for background */
    struct { int r, g, b; } color_border;       /**< RGB for border */
} Config;

/**
 * @brief Loads configuration from INI file.
 *
 * Loads all values from the given INI file and fills the Config struct.
 *
 * @param config Pointer to Config structure to fill.
 * @param path Path to INI file.
 * @return 0 on success, -1 on error.
 *
 * @note All values are loaded at runtime. No static defaults are used.
 *
 * Example:
 * @code
 * Config cfg;
 * if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *     // handle error
 * }
 * @endcode
 */
int load_config_ini(Config *config, const char *path);

#endif // CONFIG_H