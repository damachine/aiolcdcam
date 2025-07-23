/*
 * @note All display logic is implemented in display.c. Only temperature values are used in current mode.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file display.h
 * @brief LCD rendering and image upload interface for CoolerDash.
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
 * @note All display logic is implemented in display.c. Only temperature values are used in current mode.
 * @copyright (c) 2025 damachine
 * @license MIT
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

/**
 * @brief Sensor data structure for display rendering.
 *
 * @details
 * Holds temperature values for CPU and GPU. Reserved fields are for future multi-mode support (e.g. coolant temperature, usage bars).
 *
 * @note Only cpu_temp and gpu_temp are used in current display mode.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @example
 *     sensor_data_t data = { .cpu_temp = 55.0f, .gpu_temp = 48.0f };
 */
typedef struct {
    float cpu_temp; /**< CPU temperature in degrees Celsius */
    float gpu_temp; /**< GPU temperature in degrees Celsius */
    /*
     * float coolant_temp; // Reserved for future multi-mode support
     */
} sensor_data_t;

/**
 * @brief Render display based on sensor data and configuration (only default mode).
 *
 * @details
 * Renders the LCD display image using the provided sensor data and configuration. Handles drawing, saving, and uploading the image.
 *
 * @param[in] config Pointer to configuration struct (Config)
 * @param[in] data Pointer to sensor data
 * @pre config must not be NULL.
 * @pre data must not be NULL.
 * @post Display image is rendered and uploaded if successful.
 * @return 1 on success, 0 on error
 * @throws No exceptions, but returns 0 on error.
 * @note Only default mode is supported.
 * @see sensor_data_t
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @example
 *     int result = render_display(&config, &sensor_data);
 */
int render_display(const Config *config, const sensor_data_t *data);

/**
 * @brief Collects sensor data and renders display (default mode only).
 *
 * @details
 * Reads all relevant sensor data (temperatures) and renders the display image. Also uploads the image to the device if available.
 *
 * @param[in] config Pointer to configuration struct (Config)
 * @pre config must not be NULL.
 * @post Display image is rendered and uploaded if successful.
 * @return void
 * @note Only default mode is supported.
 * @see render_display
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @example
 *     draw_combined_image(&config);
 */
void draw_combined_image(const Config *config);

/**
 * @brief Calculates the color gradient for temperature bars (green → orange → red).
 *
 * @details
 * Utility function for temperature color mapping. Determines the RGB color for a given temperature value according to the defined thresholds.
 *
 * @param[in] config Pointer to configuration struct (Config)
 * @param[in] val Temperature value
 * @param[out] r Red component (0-255)
 * @param[out] g Green component (0-255)
 * @param[out] b Blue component (0-255)
 * @pre config must not be NULL.
 * @pre r, g, b must not be NULL.
 * @post r, g, b are set to the calculated color values.
 * @return void
 * @note Only temperature color mapping is supported.
 * @warning This function does not interpolate between thresholds, only assigns discrete colors.
 * @todo Add smooth interpolation for color gradients.
 * @see Config
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @since 0.25.07.23.5-1
 * @version 1.0
 * @example
 *     int r, g, b;
 *     lerp_temp_color(&config, 65.0f, &r, &g, &b);
 */
void lerp_temp_color(const Config *config, float val, int *r, int *g, int *b);

#endif /* DISPLAY_H */
