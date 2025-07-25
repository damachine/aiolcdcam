/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/**
 * @brief INI parser handler for CoolerDash configuration.
 * @details Parses the configuration file and sets values in the Config struct.
 * @example
 *     See function documentation for usage examples.
 */

// Include project headers
#include "../include/config.h"
 
// Include necessary headers
#include <ini.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief INI parser handler, sets values in Config struct.
 * @details Called for each key-value pair in the INI file. Matches section and key names and sets the corresponding field in the Config struct. Unrecognized keys are ignored.
 * @example
 *     ini_parse("/etc/coolerdash/config.ini", inih_config_handler, &cfg);
 */
static int inih_config_handler(void *user, const char *section, const char *name, const char *value)
{
    Config *config = (Config *)user;

    if (strcmp(section, "display") == 0) {
        if (strcmp(name, "width") == 0) config->display_width = atoi(value);
        else if (strcmp(name, "height") == 0) config->display_height = atoi(value);
        else if (strcmp(name, "refresh_interval_sec") == 0) config->display_refresh_interval_sec = atoi(value);
        else if (strcmp(name, "refresh_interval_nsec") == 0) config->display_refresh_interval_nsec = atoi(value);
        else if (strcmp(name, "brightness") == 0) config->lcd_brightness = atoi(value);
        else if (strcmp(name, "orientation") == 0) config->lcd_orientation = atoi(value);
    }
    else if (strcmp(section, "layout") == 0) {
        if (strcmp(name, "box_width") == 0) config->box_width = atoi(value);
        else if (strcmp(name, "box_height") == 0) config->box_height = atoi(value);
        else if (strcmp(name, "box_gap") == 0) config->box_gap = atoi(value);
        else if (strcmp(name, "bar_width") == 0) config->bar_width = atoi(value);
        else if (strcmp(name, "bar_height") == 0) config->bar_height = atoi(value);
        else if (strcmp(name, "bar_gap") == 0) config->bar_gap = atoi(value);
        else if (strcmp(name, "border_line_width") == 0) config->border_line_width = (float)atof(value);
    }
    else if (strcmp(section, "font") == 0) {
        if (strcmp(name, "face") == 0) strncpy(config->font_face, value, sizeof(config->font_face) - 1);
        else if (strcmp(name, "size_large") == 0) config->font_size_large = (float)atof(value);
        else if (strcmp(name, "size_labels") == 0) config->font_size_labels = (float)atof(value);
    }
    else if (strcmp(section, "temperature") == 0) {
        if (strcmp(name, "threshold_green") == 0) config->temp_threshold_green = (float)atof(value);
        else if (strcmp(name, "threshold_orange") == 0) config->temp_threshold_orange = (float)atof(value);
        else if (strcmp(name, "threshold_red") == 0) config->temp_threshold_red = (float)atof(value);
    }
    else if (strcmp(section, "cache") == 0) {
        if (strcmp(name, "gpu_interval") == 0) config->gpu_cache_interval = (float)atof(value);
        else if (strcmp(name, "change_tolerance_temp") == 0) config->change_tolerance_temp = (float)atof(value);
    }
    else if (strcmp(section, "paths") == 0) {
        if (strcmp(name, "hwmon") == 0) strncpy(config->hwmon_path, value, sizeof(config->hwmon_path) - 1);
        else if (strcmp(name, "image_dir") == 0) strncpy(config->image_dir, value, sizeof(config->image_dir) - 1);
        else if (strcmp(name, "image_path") == 0) strncpy(config->image_path, value, sizeof(config->image_path) - 1);
        else if (strcmp(name, "shutdown_image") == 0) strncpy(config->shutdown_image, value, sizeof(config->shutdown_image) - 1);
        else if (strcmp(name, "pid_file") == 0) strncpy(config->pid_file, value, sizeof(config->pid_file) - 1);
    }
    else if (strcmp(section, "daemon") == 0) {
        if (strcmp(name, "address") == 0) strncpy(config->daemon_address, value, sizeof(config->daemon_address) - 1);
        else if (strcmp(name, "password") == 0) strncpy(config->daemon_password, value, sizeof(config->daemon_password) - 1);
    }
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
    return 1;
}

/**
 * @brief Loads configuration from INI file.
 * @details Parses the INI file and fills the Config struct.
 * @example
 *     Config cfg;
 *     if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *         // handle error
 *     }
 */
int load_config_ini(Config *config, const char *path)
{
    if (!config || !path) return -1;
    int error = ini_parse(path, inih_config_handler, config);
    if (error < 0) {
        return -1;
    }
    return 0;
}