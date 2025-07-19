#include "../include/display.h"
#include "../include/config.h"
#include "../include/coolercontrol.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/coolant_monitor.h"
#include <cairo/cairo.h>
#include <sys/stat.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

/**
 * @brief Calculate color gradient for temperature bars (green → orange → red).
 *
 * Determines the RGB color for a given temperature value according to the defined thresholds.
 *
 * @param val Temperature value
 * @param[out] r Red component (0-255)
 * @param[out] g Green component (0-255)
 * @param[out] b Blue component (0-255)
 * @return void
 *
 * Example:
 * @code
 * int r, g, b;
 * lerp_temp_color(65.0f, &r, &g, &b);
 * @endcode
 */
void lerp_temp_color(float val, int* r, int* g, int* b) {
    if (val <= TEMP_THRESHOLD_GREEN) { 
        *r = COLOR_GREEN_R; *g = COLOR_GREEN_G; *b = COLOR_GREEN_B; 
    } else if (val <= TEMP_THRESHOLD_ORANGE) { 
        *r = COLOR_ORANGE_R; *g = COLOR_ORANGE_G; *b = COLOR_ORANGE_B; 
    } else if (val <= TEMP_THRESHOLD_RED) { 
        *r = COLOR_HOT_ORANGE_R; *g = COLOR_HOT_ORANGE_G; *b = COLOR_HOT_ORANGE_B; 
    } else { 
        *r = COLOR_RED_R; *g = COLOR_RED_G; *b = COLOR_RED_B; 
    }
}

// Forward declarations for internal functions
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data);
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data);
static void draw_labels(cairo_t *cr);
static int should_update_display(const sensor_data_t *data);

/**
 * @brief Render display based on sensor data (only default mode).
 *
 * Renders the LCD display image using the provided sensor data. Handles drawing, saving, and uploading the image.
 *
 * @param data Pointer to sensor data
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * int result = render_display(&sensor_data);
 * @endcode
 */
int render_display(const sensor_data_t *data) {
    if (!data) return 0;

    cairo_surface_t *surface = NULL;
    cairo_t *cr = NULL;
    int success = 0;

    // Check if update is needed
    if (!should_update_display(data)) {
        return 1; // No update needed, but no error
    }

    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!surface || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        goto cleanup;
    }

    cr = cairo_create(surface);
    if (!cr || cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        goto cleanup;
    }

    // Black background
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Draw labels
    draw_labels(cr);

    // Draw temperature displays
    draw_temperature_displays(cr, data);

    // Draw temperature bars
    draw_temperature_bars(cr, data);

    // Create image directory if needed
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        mkdir(IMAGE_DIR, 0755);
    }

    // Save PNG
    if (cairo_surface_write_to_png(surface, IMAGE_PATH) == CAIRO_STATUS_SUCCESS) {
        // Ensure PNG is fully written to disk before sending to LCD
        fflush(NULL);
        success = 1;

        if (is_session_initialized()) {
            const char* device_uuid = get_cached_uuid();
            if (device_uuid) {
                // Workaround: send image twice to avoid LCD artifacts
                send_image_to_lcd(IMAGE_PATH, device_uuid);
                send_image_to_lcd(IMAGE_PATH, device_uuid);
            }
        }
    }

cleanup:
    if (cr) cairo_destroy(cr);
    if (surface) cairo_surface_destroy(surface);

    return success;
}

/**
 * @brief Draw temperature displays (large numbers for CPU, GPU).
 *
 * Draws the temperature values for CPU and GPU in their respective boxes according to the 240x240px layout. CPU and GPU temperatures are centered in their boxes.
 *
 * @param cr Cairo drawing context
 * @param data Pointer to sensor data (temperatures)
 * @return void
 *
 * Example:
 * @code
 * draw_temperature_displays(cr, &sensor_data);
 * @endcode
 */
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data) {
    // Box positions for 240x240 layout, two boxes (top/bottom)
    const int cpu_box_x = 0; // top box, full width
    const int cpu_box_y = 0 ;
    const int gpu_box_x = 0; // bottom box, full width
    const int gpu_box_y = BOX_HEIGHT;
    
    // Set font and size
    cairo_select_font_face(cr, FONT_FACE, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgb(cr, COLOR_TEMP_R, COLOR_TEMP_G, COLOR_TEMP_B);
    
    char temp_str[8];
    cairo_text_extents_t ext;

    // CPU temperature display (number + degree symbol in one string)
    snprintf(temp_str, sizeof(temp_str), "%d\xC2\xB0", (int)data->cpu_temp);
    cairo_set_font_size(cr, FONT_SIZE_LARGE);
    cairo_text_extents(cr, temp_str, &ext);
    // Centered in top box (no bearing correction)
    const double cpu_temp_x = cpu_box_x + (BOX_WIDTH - ext.width) / 2 + 22;
    const double cpu_temp_y = cpu_box_y + (BOX_HEIGHT + ext.height) / 2 - 22;
    cairo_move_to(cr, cpu_temp_x, cpu_temp_y);
    cairo_show_text(cr, temp_str);

    // GPU temperature display (number + degree symbol in one string)
    snprintf(temp_str, sizeof(temp_str), "%d\xC2\xB0", (int)data->gpu_temp);
    cairo_set_font_size(cr, FONT_SIZE_LARGE);
    cairo_text_extents(cr, temp_str, &ext);
    // Centered in bottom box (no bearing correction)
    const double gpu_temp_x = gpu_box_x + (BOX_WIDTH - ext.width) / 2 + 22;
    const double gpu_temp_y = gpu_box_y + (BOX_HEIGHT + ext.height) / 2 + 22;
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
 * @return void
 *
 * Example:
 * @code
 * draw_temperature_bars(cr, &sensor_data);
 * @endcode
 */
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data) {
    // Calculate horizontal position (centered)
    const int bar_x = (DISPLAY_WIDTH - BAR_WIDTH) / 2;
    // Calculate vertical positions for CPU and GPU bars
    const int cpu_bar_y = (DISPLAY_HEIGHT - (2 * BAR_HEIGHT + BAR_GAP)) / 2 + 1;
    const int gpu_bar_y = cpu_bar_y + BAR_HEIGHT + BAR_GAP;
    
    // --- CPU bar ---
    int r, g, b;
    lerp_temp_color(data->cpu_temp, &r, &g, &b); // Get color for CPU temperature
    const int cpu_val_w = (data->cpu_temp > 0.0f) ? 
        (int)((data->cpu_temp / 100.0f) * BAR_WIDTH) : 0; // Calculate filled width
    const int safe_cpu_val_w = (cpu_val_w < 0) ? 0 : 
        (cpu_val_w > BAR_WIDTH) ? BAR_WIDTH : cpu_val_w; // Clamp to valid range
    
    // Draw CPU bar background (rounded corners)
    double radius = 8.0; // Corner radius in px
    cairo_set_source_rgb(cr, COLOR_BG_R, COLOR_BG_G, COLOR_BG_B);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, cpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw CPU bar fill (temperature color, rounded)
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    double fill_width = safe_cpu_val_w;
    cairo_new_sub_path(cr);
    if (fill_width > 2 * radius) {
        cairo_arc(cr, bar_x + fill_width - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
        cairo_arc(cr, bar_x + fill_width - radius, cpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
        cairo_arc(cr, bar_x + radius, cpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
        cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    } else {
        // If fill is too small, draw as rectangle
        cairo_rectangle(cr, bar_x, cpu_bar_y, fill_width, BAR_HEIGHT);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw CPU bar border (rounded)
    cairo_set_line_width(cr, BORDER_LINE_WIDTH);
    cairo_set_source_rgb(cr, COLOR_BORDER_R, COLOR_BORDER_G, COLOR_BORDER_B);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, cpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, cpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, cpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_stroke(cr);

    // --- GPU bar ---
    lerp_temp_color(data->gpu_temp, &r, &g, &b); // Get color for GPU temperature
    const int gpu_val_w = (data->gpu_temp > 0.0f) ? 
        (int)((data->gpu_temp / 100.0f) * BAR_WIDTH) : 0; // Calculate filled width
    const int safe_gpu_val_w = (gpu_val_w < 0) ? 0 : 
        (gpu_val_w > BAR_WIDTH) ? BAR_WIDTH : gpu_val_w; // Clamp to valid range
    // Draw GPU bar background (rounded corners)
    cairo_set_source_rgb(cr, COLOR_BG_R, COLOR_BG_G, COLOR_BG_B);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, gpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw GPU bar fill (temperature color, rounded)
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    fill_width = safe_gpu_val_w;
    cairo_new_sub_path(cr);
    if (fill_width > 2 * radius) {
        cairo_arc(cr, bar_x + fill_width - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
        cairo_arc(cr, bar_x + fill_width - radius, gpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
        cairo_arc(cr, bar_x + radius, gpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
        cairo_arc(cr, bar_x + radius, gpu_bar_y + radius, radius, M_PI, 1.5 * M_PI);
    } else {
        cairo_rectangle(cr, bar_x, gpu_bar_y, fill_width, BAR_HEIGHT);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
    // Draw GPU bar border (rounded)
    cairo_set_line_width(cr, BORDER_LINE_WIDTH);
    cairo_set_source_rgb(cr, COLOR_BORDER_R, COLOR_BORDER_G, COLOR_BORDER_B);
    cairo_new_sub_path(cr);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, gpu_bar_y + radius, radius, -M_PI_2, 0);
    cairo_arc(cr, bar_x + BAR_WIDTH - radius, gpu_bar_y + BAR_HEIGHT - radius, radius, 0, M_PI_2);
    cairo_arc(cr, bar_x + radius, gpu_bar_y + BAR_HEIGHT - radius, radius, M_PI_2, M_PI);
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
 * @return void
 *
 * Example:
 * @code
 * draw_labels(cr);
 * @endcode
 */
static void draw_labels(cairo_t *cr) {
    // Set font and size
    cairo_select_font_face(cr, FONT_FACE, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, FONT_SIZE_LABELS);
    cairo_set_source_rgb(cr, COLOR_LABEL_R, COLOR_LABEL_G, COLOR_LABEL_B);
    // CPU label: left in top box
    cairo_move_to(cr, + 0, BOX_HEIGHT / 2 + FONT_SIZE_LABELS / 2 - 12);
    cairo_show_text(cr, "CPU");
    // GPU label: left in bottom box
    cairo_move_to(cr, + 0 ,BOX_HEIGHT + BOX_HEIGHT / 2 + FONT_SIZE_LABELS / 2 +2);
    cairo_show_text(cr, "GPU");
}

/**
 * @brief Check if display update is needed (change detection).
 *
 * Compares current sensor data with last drawn values and determines if a redraw is necessary.
 *
 * @param data Pointer to current sensor data
 * @return 1 if update is needed, 0 otherwise
 *
 * Example:
 * @code
 * if (should_update_display(&sensor_data)) {
 *     // redraw
 * }
 * @endcode
 */
static int should_update_display(const sensor_data_t *data) {
    static sensor_data_t last_data = {.cpu_temp = 1.0f, .gpu_temp = 1.0f};
    static int first_run = 1;
    
    if (first_run) {
        first_run = 0;
        last_data = *data;
        return 1;
    }
    // Check for significant changes (only CPU/GPU temperatures)
    if (fabsf(data->cpu_temp - last_data.cpu_temp) > CHANGE_TOLERANCE_TEMP ||
        fabsf(data->gpu_temp - last_data.gpu_temp) > CHANGE_TOLERANCE_TEMP) {
        last_data = *data;
        return 1;
    }
    return 0;
}

/**
 * @brief Collects sensor data and renders display (default mode only).
 *
 * Reads all relevant sensor data (temperatures) and renders the display image. Also uploads the image to the device if available.
 *
 * @return void
 *
 * Example:
 * @code
 * draw_combined_image();
 * @endcode
 */
void draw_combined_image(void) {
    sensor_data_t sensor_data = {0};
    // Temperatures
    sensor_data.cpu_temp = read_cpu_temp();
    sensor_data.gpu_temp = read_gpu_temp();
    // Render display
    int render_result = render_display(&sensor_data);
    if (render_result == 0) {
        // Silent continuation on render errors
        return;
    }
}
