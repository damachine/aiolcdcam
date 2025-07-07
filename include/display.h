#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// =============================================================================
// DISPLAY RENDERING MODULE
// =============================================================================

// Display Mode Types
typedef enum {
    DISPLAY_MODE_DEF,  // Nur Temperaturen
    DISPLAY_MODE_1,    // Temperaturen + vertikale Balken
    DISPLAY_MODE_2,    // Temperaturen + Kreisdiagramme
    DISPLAY_MODE_3     // Temperaturen + horizontale Balken
} display_mode_t;

// Sensor Data Structure
typedef struct {
    float cpu_temp;
    float gpu_temp;
    float coolant_temp;
    float cpu_usage;    // Nur für Modi 1, 2, 3
    float ram_usage;    // Nur für Modi 1, 2, 3
    float gpu_usage;    // Nur für Modi 1, 2, 3
    float gpu_ram_usage; // Nur für Modi 1, 2, 3
} sensor_data_t;

// Display Functions
display_mode_t parse_display_mode(const char *mode_str);
int render_display(const sensor_data_t *data, display_mode_t mode);
void draw_combined_image(display_mode_t mode);  // Vereinfachte All-in-One Funktion

// Utility Functions
void lerp_temp_color(float val, int* r, int* g, int* b);

#endif // DISPLAY_H
