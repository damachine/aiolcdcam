/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file config.h
 * @brief Central configuration header for CoolerDash.
 *
 * @details
 * This header defines the configuration structures and function prototypes for CoolerDash.
 * It enforces coding and documentation standards:
 * - All code comments are in English and use Doxygen style for functions.
 * - Opening braces for functions and control structures are placed on the same line (K&R style).
 * - All non-obvious code sections are commented.
 * - Redundant comments are avoided.
 * - Complex algorithms and data structures are documented in detail.
 *
 * C99 Coding Guidelines:
 * - Follows ISO/IEC 9899:1999 (C99)
 * - Checks return values of malloc(), calloc(), realloc()
 * - Frees dynamic memory and sets pointers to NULL
 * - Uses include guards: #ifndef CONFIG_H / #define CONFIG_H / #endif
 * - Only necessary headers are included; system and local headers are separated
 * - Indentation is 4 spaces, no tabs
 * - Uses const for immutable variables and parameters
 * - Uses static for file-local functions/variables
 * - Uses inline for small, frequently used functions
 *
 * Naming Conventions:
 * - Functions: snake_case verbs (e.g. calculate_sum())
 * - Variables: snake_case (e.g. user_count)
 * - Constants/Macros: UPPER_CASE (e.g. MAX_SIZE)
 * - Structs via typedef: PascalCase (e.g. MyStruct)
 * - Descriptive names, no abbreviations
 * - Uses enum for status/error codes
 * - Uses typedef for complex types
 * - Consistent naming throughout the project
 *
 * @note All configuration values are loaded exclusively from the INI file at runtime.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <ini.h>

/**
 * @brief Color struct for RGB values (0-255).
 *
 * @details Used for all color configuration values in CoolerDash.
 * @author damachine
 * @since 0.25.07.23.5-1
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
typedef struct {
    int r; /**< Red value (0-255) */
    int g; /**< Green value (0-255) */
    int b; /**< Blue value (0-255) */
} Color;

/**
 * @brief Structure for runtime configuration loaded from INI file.
 *
 * @details
 * All fields are loaded from the INI file. No static defaults are defined here.
 * - All display, layout, font, temperature, cache, path and color settings are loaded at runtime.
 * - Color fields use int (0-255) for RGB values.
 * - All string fields are zero-terminated and sized for safe usage.
 *
 * @note This struct is used throughout the application for configuration.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
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
    Color color_green;        /**< RGB for green bar */
    Color color_orange;       /**< RGB for orange bar */
    Color color_hot_orange;   /**< RGB for hot orange bar */
    Color color_red;          /**< RGB for red bar */
    Color color_temp;         /**< RGB for temperature text */
    Color color_label;        /**< RGB for label text */
    Color color_bg;           /**< RGB for background */
    Color color_border;       /**< RGB for border */
} Config;

/**
 * @brief Loads configuration from INI file.
 *
 * @details
 * Loads all configuration values from the specified INI file and populates the given Config structure.
 * All fields are loaded at runtime; no static defaults are used.
 *
 * @param[in,out] config Pointer to Config structure to fill.
 * @param[in] path Path to INI file.
 * @pre config must not be NULL.
 * @pre path must point to a valid, readable INI file.
 * @post config is filled with values from the INI file if return is 0.
 * @return 0 on success, -1 on error.
 * @throws No exceptions, but returns -1 on file or parse errors.
 * @note All configuration values are loaded at runtime.
 * @warning If the INI file is malformed or missing fields, the function may leave config in an undefined state.
 * @bug Does not validate all field ranges; see issue #42.
 * @todo Add validation for all numeric fields.
 * @see Config
 * @example
 *     Config cfg;
 *     if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *         // handle error
 *     }
 * @deprecated Use load_config_yaml() for YAML support.
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 */
int load_config_ini(Config *config, const char *path);

#endif // CONFIG_H