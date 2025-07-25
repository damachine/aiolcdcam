/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/**
 * @brief CoolerControl API interface for LCD device communication.
 * @details Provides functions for initializing, authenticating, and communicating with CoolerControl LCD devices.
 * @example
 *     See function documentation for usage examples.
 */

// Function prototypes
#ifndef CONFIG_H
#define CONFIG_H

// Include necessary headers
#include <stdint.h>
#include <ini.h>

/**
 * @brief Color struct for RGB values (0-255).
 * @details Used for all color configuration values in CoolerDash.
 * @example
 *     Color green = {0, 255, 0};
 *     Color red = {255, 0, 0};
 *     // Use in config: cfg.color_green = green;
 */
typedef struct {
    int r; // Red value (0-255)
    int g; // Green value (0-255)
    int b; // Blue value (0-255)
} Color;

/**
 * @brief Structure for runtime configuration loaded from INI file.
 * @details All fields are loaded from the INI file.
 * @example
 *     Config cfg;
 *     if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") == 0) {
 *         // Use cfg fields, e.g. cfg.display_width
 *     }
 */
typedef struct Config {
    int display_width;           // Display width in pixels
    int display_height;          // Display height in pixels
    int display_refresh_interval_sec; // Refresh interval (seconds)
    int display_refresh_interval_nsec; // Refresh interval (nanoseconds)
    int lcd_brightness;          // LCD brightness (0-100)
    int lcd_orientation;         // LCD orientation: 0, 90, 180, 270
    int box_width;               // Box width in pixels
    int box_height;              // Box height in pixels
    int box_gap;                 // Gap between boxes in pixels
    int bar_width;               // Bar width in pixels
    int bar_height;              // Bar height in pixels
    int bar_gap;                 // Gap between bars in pixels
    float border_line_width;     // Border line width in pixels
    char font_face[64];          // Font face for display text
    float font_size_large;       // Large font size
    float font_size_labels;      // Label font size
    float temp_threshold_green;  // Green threshold (°C)
    float temp_threshold_orange; // Orange threshold (°C)
    float temp_threshold_red;    // Red threshold (°C)
    float gpu_cache_interval;    // GPU cache interval (seconds)
    float change_tolerance_temp; // Temperature change tolerance (°C)
    char hwmon_path[128];        // Path to hwmon
    char image_dir[128];         // Directory for images
    char image_path[128];        // Path for display image
    char shutdown_image[128];    // Path for shutdown image
    char pid_file[128];          // Path for PID file
    char daemon_address[128];    // Daemon address
    char daemon_password[64];    // Daemon password
    Color color_green;        // RGB for green bar
    Color color_orange;       // RGB for orange bar
    Color color_hot_orange;   // RGB for hot orange bar
    Color color_red;          // RGB for red bar
    Color color_temp;         // RGB for temperature text
    Color color_label;        // RGB for label text
    Color color_bg;           // RGB for background
    Color color_border;       // RGB for border
} Config;

/**
 * @brief Loads configuration from INI file.
 * @details Loads all configuration values from the specified INI file and populates the given Config structure.
 * @example
 *     Config cfg;
 *     if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *         // handle error
 *     }
 */
int load_config_ini(Config *config, const char *path);

#endif // CONFIG_H