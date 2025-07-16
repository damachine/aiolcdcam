#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// =============================================================================
// DISPLAY RENDERING MODULE
// =============================================================================

// Display Mode Types
typedef enum {
    DISPLAY_MODE_DEF,  // Temperatures only
    DISPLAY_MODE_1,    // Temperatures + vertical bars
    DISPLAY_MODE_2,    // Temperatures + circle diagrams
    DISPLAY_MODE_3     // Temperatures + horizontal bars
} display_mode_t;

// Sensor Data Structure
typedef struct {
    float cpu_temp;
    float gpu_temp;
    float coolant_temp;
    float cpu_usage;    // Only for modes 1, 2, 3
    float ram_usage;    // Only for modes 1, 2, 3
    float gpu_usage;    // Only for modes 1, 2, 3
    float gpu_ram_usage; // Only for modes 1, 2, 3
} sensor_data_t;

// Display Functions
/**
 * Converts a mode string to the corresponding display mode enum.
 * @param mode_str Mode string ("def", "1", "2", "3")
 * @return Display mode enum value
 * @example
 *   display_mode_t mode = parse_display_mode("1");
 */
display_mode_t parse_display_mode(const char *mode_str);

/**
 * Renders the display image based on sensor data and display mode.
 * @param data Pointer to sensor data structure
 * @param mode Display mode enum
 * @return 1 on success, 0 on error
 * @brief Creates and saves the LCD image for the current sensor state.
 */
int render_display(const sensor_data_t *data, display_mode_t mode);

/**
 * Collects sensor data and renders the display in one step.
 * @param mode Display mode enum
 * @brief Simplified all-in-one function for updating the LCD image.
 */
void draw_combined_image(display_mode_t mode);

/**
 * Calculates the color gradient for temperature bars (green → orange → red).
 * @param val Temperature value
 * @param[out] r Red component (0-255)
 * @param[out] g Green component (0-255)
 * @param[out] b Blue component (0-255)
 * @brief Utility function for temperature color mapping.
 */
void lerp_temp_color(float val, int* r, int* g, int* b);

#endif // DISPLAY_H
