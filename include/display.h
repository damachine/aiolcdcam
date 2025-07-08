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
display_mode_t parse_display_mode(const char *mode_str);
int render_display(const sensor_data_t *data, display_mode_t mode);
void draw_combined_image(display_mode_t mode);  // Simplified all-in-one function

// Utility Functions
void lerp_temp_color(float val, int* r, int* g, int* b);

#endif // DISPLAY_H
