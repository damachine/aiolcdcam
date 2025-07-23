/*
 * @note All display logic is implemented in display.c. Only temperature values are used in current mode.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file display.c
 * @brief LCD rendering and image upload for CoolerDash.
 *
 * @details
 * Implements all display rendering logic, including temperature bars, labels, and image upload.
 *
 * Coding and Documentation Standards for CoolerDash:
 * - All code comments are written in English.
 * - Doxygen style is used for all function comments.
 * - See coding standards in project documentation and config.h for details.
 * - Opening braces for functions and control structures are placed on the same line (K&R style).
 * - Only necessary headers are included; system and local headers are separated.
 * - Code is indented with 4 spaces, no tabs.
 * - All functions, variables, and types follow project naming conventions (snake_case, PascalCase, UPPER_CASE).
 * - Complex algorithms and data structures are documented in detail.
 * - Inline comments are used sparingly and only when necessary.
 * - Redundant comments are avoided.
 * - All dynamically allocated memory is freed and pointers set to NULL.
 * - All malloc/calloc/realloc return values are checked.
 * - Only temperature logic is present; all usage/statistics code has been removed.
 * - Single instance enforcement is handled externally.
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
 * @see config.h, display.h
 * @todo Add support for additional display modes if required.
 * @example See function documentation for usage examples.
 */

// Include project headers
#include "../include/display.h"
#include "../include/config.h"
#include "../include/coolercontrol.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"

 // Include necessary headers
#include <cairo/cairo.h>
#include <sys/stat.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

// Constants for display layout
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

/**
 * @brief Calculate color gradient for temperature bars (green → orange → red).
 *
 * Determines the RGB color for a given temperature value according to the defined thresholds from config.
 *
 * @param config Pointer to configuration struct (Config)
 * @param val Temperature value
 * @param[out] r Red component (0-255)
 * @param[out] g Green component (0-255)
 * @param[out] b Blue component (0-255)
 * @return void
 *
 * Example:
 * @code
 * int r, g, b;
 * lerp_temp_color(&config, 65.0f, &r, &g, &b);
 * @endcode
 */
void lerp_temp_color(const Config *config, float val, int* r, int* g, int* b) {
    if (val <= config->temp_threshold_green) { 
        *r = config->color_green.r; *g = config->color_green.g; *b = config->color_green.b; 
    } else if (val <= config->temp_threshold_orange) { 
        *r = config->color_orange.r; *g = config->color_orange.g; *b = config->color_orange.b; 
    } else if (val <= config->temp_threshold_red) { 
        *r = config->color_hot_orange.r; *g = config->color_hot_orange.g; *b = config->color_hot_orange.b; 
    } else { 
        *r = config->color_red.r; *g = config->color_red.g; *b = config->color_red.b; 
    }
}

// Forward declarations for internal functions
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data, const Config *config);
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data, const Config *config);
static void draw_labels(cairo_t *cr, const Config *config);
static int should_update_display(const sensor_data_t *data, const Config *config);

/**
 * @brief Render display based on sensor data (only default mode).
 *
 * Renders the LCD display image using the provided sensor data. Handles drawing, saving, and uploading the image.
 * Checks if an update is needed, creates the Cairo surface, draws all elements, saves the PNG, and uploads to LCD if available.
 *
 * @param config Pointer to configuration struct (Config)
 * @param data Pointer to sensor data
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * int result = render_display(&config, &sensor_data);
 * @endcode
 */
int render_display(const Config *config, const sensor_data_t *data) {
    if (!data || !config) return 0;

    cairo_surface_t *surface = NULL;
    cairo_t *cr = NULL;
    int success = 0;

    // Only update if sensor data changed significantly
    if (!should_update_display(data, config)) {
        return 1; // No update needed, but no error
    }

    // Create Cairo surface for drawing
    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, config->display_width, config->display_height);
    if (!surface || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        goto cleanup;
    }

    cr = cairo_create(surface);
    if (!cr || cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        goto cleanup;
    }

    // Fill background with black
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Draw labels (CPU/GPU)
    draw_labels(cr, config);

    // Draw temperature values
    draw_temperature_displays(cr, data, config);

    // Draw temperature bars
    draw_temperature_bars(cr, data, config);

    // Ensure image directory exists
    struct stat st = {0};
    if (stat(config->image_dir, &st) == -1) {
        mkdir(config->image_dir, 0755);
    }

    // Save PNG image
    if (cairo_surface_write_to_png(surface, config->image_path) == CAIRO_STATUS_SUCCESS) {
        fflush(NULL); // Ensure PNG is written before upload
        success = 1;

        // Upload image to LCD if session is initialized
        if (is_session_initialized()) {
            const char* device_uid = get_cached_device_uid();
            if (device_uid[0]) {
                // Send image to LCD (double send for reliability)
                send_image_to_lcd(config, config->image_path, device_uid);
                send_image_to_lcd(config, config->image_path, device_uid);
            }
        }
    }

cleanup:
    // Free Cairo resources
    if (cr) {
        cairo_destroy(cr);
        cr = NULL;
    }
    if (surface) {
        cairo_surface_destroy(surface);
        surface = NULL;
    }

    return success;
}

/**
 * @brief Draw temperature displays (large numbers for CPU, GPU).
 *
 * Draws the temperature values for CPU and GPU in their respective boxes according to the 240x240px layout. CPU and GPU temperatures are centered in their boxes.
 *
 * @param cr Cairo drawing context
 * @param data Pointer to sensor data (temperatures)
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * draw_temperature_displays(cr, &sensor_data);
 * @endcode
 */
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data, const Config *config) {
    // Box positions for 240x240 layout, two boxes (top/bottom)
    const int cpu_box_x = 0; // top box, full width
    const int cpu_box_y = 0 ;
    const int gpu_box_x = 0; // bottom box, full width
    const int gpu_box_y = config->box_height;
    
    // Set font and size
    cairo_select_font_face(cr, config->font_face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgb(cr, config->color_temp.r / 255.0, config->color_temp.g / 255.0, config->color_temp.b / 255.0);

    char temp_str[8];
    cairo_text_extents_t ext;

    // CPU temperature display (number + degree symbol in one string)
    snprintf(temp_str, sizeof(temp_str), "%d\xC2\xB0", (int)data->cpu_temp);
    cairo_set_font_size(cr, config->font_size_large);
    cairo_text_extents(cr, temp_str, &ext);
    // Centered in top box (no bearing correction)
    const double cpu_temp_x = cpu_box_x + (config->box_width - ext.width) / 2 + 22;
    const double cpu_temp_y = cpu_box_y + (config->box_height + ext.height) / 2 - 22;
    cairo_move_to(cr, cpu_temp_x, cpu_temp_y);
    cairo_show_text(cr, temp_str);

    // GPU temperature display (number + degree symbol in one string)
    snprintf(temp_str, sizeof(temp_str), "%d\xC2\xB0", (int)data->gpu_temp);
    cairo_set_font_size(cr, config->font_size_large);
    cairo_text_extents(cr, temp_str, &ext);
    // Centered in bottom box (no bearing correction)
    const double gpu_temp_x = gpu_box_x + (config->box_width - ext.width) / 2 + 22;
    const double gpu_temp_y = gpu_box_y + (config->box_height + ext.height) / 2 + 22;
    cairo_move_to(cr, gpu_temp_x, gpu_temp_y);
    cairo_show_text(cr, temp_str);
}

/**
 * @brief Draw temperature bars (CPU and GPU).
 *
 * Draws horizontal bars representing CPU and GPU temperatures, with color gradient according to temperature value.
 *
 * @param cr Cairo drawing context
 * @param data Pointer to sensor data (temperatures)
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * draw_temperature_bars(cr, &sensor_data);
 * @endcode
 */
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data, const Config *config) {
    // Calculate horizontal position (centered)
    const int bar_x = (config->display_width - config->bar_width) / 2;
    // Calculate vertical positions for CPU and GPU bars
    const int cpu_bar_y = (config->display_height - (2 * config->bar_height + config->bar_gap)) / 2 + 1;
    const int gpu_bar_y = cpu_bar_y + config->bar_height + config->bar_gap;
    
    // --- CPU bar ---
    int r, g, b;
    lerp_temp_color(config, data->cpu_temp, &r, &g, &b); // Get color for CPU temperature
    const int cpu_val_w = (data->cpu_temp > 0.0f) ? 
        (int)((data->cpu_temp / 100.0f) * config->bar_width) : 0; // Calculate filled width
    const int safe_cpu_val_w = (cpu_val_w < 0) ? 0 : 
        (cpu_val_w > config->bar_width) ? config->bar_width : cpu_val_w; // Clamp to valid range
    
    // Draw CPU bar background (rounded corners)
    /**
     * @brief Draws the background for the temperature bars.
     *
     * @details
     * Uses the RGB values from config->color_bg, normalized to 0.0–1.0 for Cairo.
     *
     * @param[in] cr Cairo drawing context
     * @param[in] config Pointer to configuration struct (Config)
     * @pre cr must not be NULL.
     * @pre config must not be NULL.
     * @post Background is drawn for the temperature bars.
     * @return void
     * @author damachine
     * @copyright (c) 2025 damachine
     * @license MIT
     * @since 0.25.07.23.5-1
     * @version 1.0
     * @note This is an internal helper for draw_temperature_bars.
     */
    double radius = 8.0; // Corner radius in px
    cairo_set_source_rgb(cr, config->color_bg.r / 255.0, config->color_bg.g / 255.0, config->color_bg.b / 255.0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + config->bar_width - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + config->bar_width - radius, cpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw CPU bar fill (temperature color, rounded)
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    double fill_width = safe_cpu_val_w;
    cairo_new_sub_path(cr);
    if (fill_width > 2 * radius) {
        cairo_arc(cr, bar_x + fill_width - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
        cairo_arc(cr, bar_x + fill_width - radius, cpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
        cairo_arc(cr, bar_x + radius, cpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
        cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    } else {
        // If fill is too small, draw as rectangle
        cairo_rectangle(cr, bar_x, cpu_bar_y, fill_width, config->bar_height);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw CPU bar border (rounded)
    cairo_set_line_width(cr, config->border_line_width);
    cairo_set_source_rgb(cr, config->color_border.r / 255.0, config->color_border.g / 255.0, config->color_border.b / 255.0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + config->bar_width - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + config->bar_width - radius, cpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_stroke(cr);

    // --- GPU bar ---
    lerp_temp_color(config, data->gpu_temp, &r, &g, &b); // Get color for GPU temperature
    const int gpu_val_w = (data->gpu_temp > 0.0f) ? 
        (int)((data->gpu_temp / 100.0f) * config->bar_width) : 0; // Calculate filled width
    const int safe_gpu_val_w = (gpu_val_w < 0) ? 0 : 
        (gpu_val_w > config->bar_width) ? config->bar_width : gpu_val_w; // Clamp to valid range
    // Draw GPU bar background (rounded corners)
    cairo_set_source_rgb(cr, config->color_bg.r / 255.0, config->color_bg.g / 255.0, config->color_bg.b / 255.0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + config->bar_width - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + config->bar_width - radius, gpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw GPU bar fill (temperature color, rounded)
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    fill_width = safe_gpu_val_w;
    cairo_new_sub_path(cr);
    if (fill_width > 2 * radius) {
        cairo_arc(cr, bar_x + fill_width - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
        cairo_arc(cr, bar_x + fill_width - radius, gpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
        cairo_arc(cr, bar_x + radius, gpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
        cairo_arc(cr, bar_x + radius, gpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    } else {
        cairo_rectangle(cr, bar_x, gpu_bar_y, fill_width, config->bar_height);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw GPU bar border (rounded)
    cairo_set_line_width(cr, config->border_line_width);
    cairo_set_source_rgb(cr, config->color_border.r / 255.0, config->color_border.g / 255.0, config->color_border.b / 255.0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + config->bar_width - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + config->bar_width - radius, gpu_bar_y + config->bar_height - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + config->bar_height - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_stroke(cr);
}

/**
 * @brief Draw CPU/GPU labels (default mode only).
 *
 * Draws text labels for CPU and GPU in the default style.
 *
 * @param cr Cairo drawing context
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * draw_labels(cr);
 * @endcode
 */
static void draw_labels(cairo_t *cr, const Config *config) {
    // Set font and size
    cairo_select_font_face(cr, config->font_face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, config->font_size_labels);
    cairo_set_source_rgb(cr, config->color_label.r / 255.0, config->color_label.g / 255.0, config->color_label.b / 255.0);
    // CPU label: left in top box
    cairo_move_to(cr, + 0, config->box_height / 2 + config->font_size_labels / 2 - 12);
    cairo_show_text(cr, "CPU");
    // GPU label: left in bottom box
    cairo_move_to(cr, + 0 ,config->box_height + config->box_height / 2 + config->font_size_labels / 2 +2);
    cairo_show_text(cr, "GPU");
}

/**
 * @brief Check if display update is needed (change detection).
 *
 * Compares current sensor data with last drawn values and determines if a redraw is necessary.
 *
 * @param data Pointer to current sensor data
 * @param config Pointer to configuration struct (Config)
 * @return 1 if update is needed, 0 otherwise
 *
 * Example:
 * @code
 * if (should_update_display(&sensor_data, config)) {
 *     // redraw
 * }
 * @endcode
 */
static int should_update_display(const sensor_data_t *data, const Config *config) {
    static sensor_data_t last_data = {.cpu_temp = -1.0f, .gpu_temp = -1.0f};
    static int first_run = 1;
    if (first_run) {
        first_run = 0;
        last_data = *data;
        return 1;
    }
    // Check for significant changes (only CPU/GPU temperatures)
    // Uses >= so that a change of exactly config->change_tolerance_temp triggers an update
    if (fabsf(data->cpu_temp - last_data.cpu_temp) >= config->change_tolerance_temp ||
        fabsf(data->gpu_temp - last_data.gpu_temp) >= config->change_tolerance_temp) {
        last_data = *data;
        return 1;
    }
    return 0;
}

/**
 * @brief Collects sensor data and renders display (default mode only).
 *
 * @details
 * Reads all relevant sensor data (CPU and GPU temperatures) and renders the display image. Also uploads the image to the device if available.
 * This function is the main entry point for display updates in default mode.
 *
 * @param[in] config Pointer to configuration struct (Config)
 * @pre config must not be NULL.
 * @post Display image is rendered and uploaded if successful.
 * @return void
 * @note Silent continuation on render errors; no error reporting to caller.
 * @see render_display
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @example
 *     draw_combined_image(&config);
 * @todo Add support for additional display modes and error reporting.
 */
void draw_combined_image(const Config *config) {
    sensor_data_t sensor_data = {0};
    // Temperatures
    sensor_data.cpu_temp = read_cpu_temp();
    sensor_data.gpu_temp = read_gpu_temp(config);
    // Render display
    int render_result = render_display(config, &sensor_data);
    if (render_result == 0) {
        // Silent continuation on render errors
        return;
    }
}
