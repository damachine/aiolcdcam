/**
 * @file config.c
 * @brief Central configuration implementation for CoolerDash.
 */

// Include project headers
#include <ini.h>
#include "../include/config.h"

// Include necessary headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Handler function for INI parser, sets values in Config struct.
 *
 * This function is called for each key-value pair in the INI file. It matches
 * section and key names and sets the corresponding field in the Config struct.
 *
 * @param user Pointer to Config struct
 * @param section Current section name
 * @param name Current key name
 * @param value Current value as string
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * ini_parse("/etc/coolerdash/config.ini", inih_config_handler, &cfg);
 * @endcode
 */
static int inih_config_handler(void* user, const char* section, const char* name, const char* value)
{
    Config* config = (Config*)user;

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
        if (strcmp(name, "face") == 0) strncpy(config->font_face, value, sizeof(config->font_face)-1);
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
        else if (strcmp(name, "change_tolerance_usage") == 0) config->change_tolerance_usage = (float)atof(value);
    }
    // Paths section
    else if (strcmp(section, "paths") == 0) {
        if (strcmp(name, "hwmon") == 0) strncpy(config->hwmon_path, value, sizeof(config->hwmon_path)-1);
        else if (strcmp(name, "image_dir") == 0) strncpy(config->image_dir, value, sizeof(config->image_dir)-1);
        else if (strcmp(name, "image_path") == 0) strncpy(config->image_path, value, sizeof(config->image_path)-1);
        else if (strcmp(name, "shutdown_image") == 0) strncpy(config->shutdown_image, value, sizeof(config->shutdown_image)-1);
        else if (strcmp(name, "pid_file") == 0) strncpy(config->pid_file, value, sizeof(config->pid_file)-1);
    }
    // Daemon section
    else if (strcmp(section, "daemon") == 0) {
        if (strcmp(name, "address") == 0) strncpy(config->daemon_address, value, sizeof(config->daemon_address)-1);
        else if (strcmp(name, "password") == 0) strncpy(config->daemon_password, value, sizeof(config->daemon_password)-1);
    }
    // Color sections (example for color_green, analog for other colors)
    else if (strcmp(section, "color_green") == 0) {
        if (strcmp(name, "r") == 0) config->color_green.r = atoi(value);
        else if (strcmp(name, "g") == 0) config->color_green.g = atoi(value);
        else if (strcmp(name, "b") == 0) config->color_green.b = atoi(value);
    }
    // TODO: Add handlers for all other color sections (color_orange, color_hot_orange, ...)
    // Each color section should set r, g, b fields accordingly.

    return 1; // success
}

/**
 * @brief Set default values for Config struct from config.h macros.
 *
 * This function initializes all fields in the Config struct with static macro defaults.
 *
 * @param config Pointer to Config struct to initialize
 */
void set_config_defaults(Config* config)
{
    if (!config) return;
    config->display_width = DISPLAY_WIDTH;
    config->display_height = DISPLAY_HEIGHT;
    config->display_refresh_interval_sec = DISPLAY_REFRESH_INTERVAL_SEC;
    config->display_refresh_interval_nsec = DISPLAY_REFRESH_INTERVAL_NSEC;
    config->lcd_brightness = LCD_BRIGHTNESS;
    config->lcd_orientation = atoi(LCD_ORIENTATION);
    config->box_width = BOX_WIDTH;
    config->box_height = BOX_HEIGHT;
    config->box_gap = BOX_GAP;
    config->bar_width = BAR_WIDTH;
    config->bar_height = BAR_HEIGHT;
    config->bar_gap = BAR_GAP;
    config->border_line_width = BORDER_LINE_WIDTH;
    strncpy(config->font_face, FONT_FACE, sizeof(config->font_face)-1);
    config->font_size_large = FONT_SIZE_LARGE;
    config->font_size_labels = FONT_SIZE_LABELS;
    config->temp_threshold_green = TEMP_THRESHOLD_GREEN;
    config->temp_threshold_orange = TEMP_THRESHOLD_ORANGE;
    config->temp_threshold_red = TEMP_THRESHOLD_RED;
    config->gpu_cache_interval = GPU_CACHE_INTERVAL;
    config->change_tolerance_temp = CHANGE_TOLERANCE_TEMP;
    config->change_tolerance_usage = CHANGE_TOLERANCE_USAGE;
    strncpy(config->hwmon_path, HWMON_PATH, sizeof(config->hwmon_path)-1);
    strncpy(config->image_dir, IMAGE_DIR, sizeof(config->image_dir)-1);
    strncpy(config->image_path, IMAGE_PATH, sizeof(config->image_path)-1);
    strncpy(config->shutdown_image, SHUTDOWN_IMAGE_PATH, sizeof(config->shutdown_image)-1);
    strncpy(config->pid_file, PID_FILE, sizeof(config->pid_file)-1);
    strncpy(config->daemon_address, DAEMON_ADDRESS, sizeof(config->daemon_address)-1);
    strncpy(config->daemon_password, DAEMON_PASSWORD, sizeof(config->daemon_password)-1);
    config->color_green.r = COLOR_GREEN_R;
    config->color_green.g = COLOR_GREEN_G;
    config->color_green.b = COLOR_GREEN_B;
    config->color_orange.r = COLOR_ORANGE_R;
    config->color_orange.g = COLOR_ORANGE_G;
    config->color_orange.b = COLOR_ORANGE_B;
    config->color_hot_orange.r = COLOR_HOT_ORANGE_R;
    config->color_hot_orange.g = COLOR_HOT_ORANGE_G;
    config->color_hot_orange.b = COLOR_HOT_ORANGE_B;
    config->color_red.r = COLOR_RED_R;
    config->color_red.g = COLOR_RED_G;
    config->color_red.b = COLOR_RED_B;
    config->color_bg.r = COLOR_BG_R;
    config->color_bg.g = COLOR_BG_G;
    config->color_bg.b = COLOR_BG_B;
    config->color_border.r = COLOR_BORDER_R;
    config->color_border.g = COLOR_BORDER_G;
    config->color_border.b = COLOR_BORDER_B;
    config->color_cpu_usage.r = COLOR_CPU_USAGE_R;
    config->color_cpu_usage.g = COLOR_CPU_USAGE_G;
    config->color_cpu_usage.b = COLOR_CPU_USAGE_B;
    config->color_ram_usage.r = COLOR_RAM_USAGE_R;
    config->color_ram_usage.g = COLOR_RAM_USAGE_G;
    config->color_ram_usage.b = COLOR_RAM_USAGE_B;
    config->color_gpu_usage.r = COLOR_GPU_USAGE_R;
    config->color_gpu_usage.g = COLOR_GPU_USAGE_G;
    config->color_gpu_usage.b = COLOR_GPU_USAGE_B;
    config->color_gpu_ram_usage.r = COLOR_GPU_RAM_USAGE_R;
    config->color_gpu_ram_usage.g = COLOR_GPU_RAM_USAGE_G;
    config->color_gpu_ram_usage.b = COLOR_GPU_RAM_USAGE_B;
    config->color_gpu_usage_yellow.r = COLOR_GPU_USAGE_YELLOW_R;
    config->color_gpu_usage_yellow.g = COLOR_GPU_USAGE_YELLOW_G;
    config->color_gpu_usage_yellow.b = COLOR_GPU_USAGE_YELLOW_B;
    config->color_temp.r = COLOR_TEMP_R;
    config->color_temp.g = COLOR_TEMP_G;
    config->color_temp.b = COLOR_TEMP_B;
    config->color_label.r = COLOR_LABEL_R;
    config->color_label.g = COLOR_LABEL_G;
    config->color_label.b = COLOR_LABEL_B;
}

/**
 * @brief Loads configuration from INI file and overwrites defaults.
 *
 * This function sets default values, then parses the INI file and overwrites
 * any values found. If the file cannot be read, defaults remain active.
 *
 * @param config Pointer to Config struct to fill
 * @param path Path to INI file
 * @return 0 on success, -1 on error
 *
 * Example:
 * @code
 * Config cfg;
 * if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *     // fallback to defaults
 * }
 * @endcode
 */
int load_config_ini(Config* config, const char* path)
{
    // Check pointer
    if (!config || !path) return -1;

    set_config_defaults(config); // Set fallback values

    // Parse INI file
    int error = ini_parse(path, inih_config_handler, config);
    if (error < 0) {
        // File not found or parse error
        return -1;
    }
    return 0;
}