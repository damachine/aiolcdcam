/**
 * @file display.h
 * @brief LCD rendering and image upload interface for CoolerDash.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

/**
 * @brief Sensor data structure for display rendering.
 *
 * Holds temperature values for CPU and GPU. Reserved fields are for future multi-mode support (e.g. coolant temperature, usage bars).
 *
 * @note Only cpu_temp and gpu_temp are used in current display mode.
 *
 * Example:
 * @code
 * sensor_data_t data = { .cpu_temp = 55.0f, .gpu_temp = 48.0f };
 * @endcode
 */
typedef struct {
    float cpu_temp; /**< CPU temperature in degrees Celsius */
    float gpu_temp; /**< GPU temperature in degrees Celsius */
    /*
     * float coolant_temp; // Reserved for future multi-mode support
     * float cpu_usage;    // Reserved for future usage bar support
     * float ram_usage;    // Reserved for future usage bar support
     * float gpu_usage;    // Reserved for future usage bar support
     * float gpu_ram_usage;// Reserved for future usage bar support
     */
} sensor_data_t;

/**
 * @brief Render display based on sensor data and configuration (only default mode).
 *
 * Renders the LCD display image using the provided sensor data and configuration. Handles drawing, saving, and uploading the image.
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
int render_display(const Config *config, const sensor_data_t *data);

/**
 * @brief Collects sensor data and renders display (default mode only).
 *
 * Reads all relevant sensor data (temperatures) and renders the display image. Also uploads the image to the device if available.
 *
 * @param config Pointer to configuration struct (Config)
 * @return void
 *
 * Example:
 * @code
 * draw_combined_image(&config);
 * @endcode
 */
void draw_combined_image(const Config *config);

/**
 * @brief Calculates the color gradient for temperature bars (green → orange → red).
 *
 * Utility function for temperature color mapping. Determines the RGB color for a given temperature value according to the defined thresholds.
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
void lerp_temp_color(const Config *config, float val, int *r, int *g, int *b);

#endif /* DISPLAY_H */
