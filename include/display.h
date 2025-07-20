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
 * Holds temperature values for CPU and GPU. Fields for coolant temperature and usage are reserved for future multi-mode support.
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
     * float coolant_temp; // Unused in current display mode (legacy: multi-mode support)
     * float cpu_usage;
     * float ram_usage;
     * float gpu_usage;
     * float gpu_ram_usage;
     */
} sensor_data_t;

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
int render_display(const sensor_data_t *data);

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
void draw_combined_image(void);

/**
 * @brief Calculates the color gradient for temperature bars (green → orange → red).
 *
 * Utility function for temperature color mapping. Determines the RGB color for a given temperature value according to the defined thresholds.
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
void lerp_temp_color(float val, int* r, int* g, int* b);

#endif // DISPLAY_H
