#ifndef COOLANT_MONITOR_H
#define COOLANT_MONITOR_H

// =============================================================================
// COOLANT TEMPERATURE MONITORING MODULE
// =============================================================================

// Coolant Temperature Functions
/**
 * @brief Initialize the coolant temperature sensor path.
 *
 * This function detects and sets the path to the coolant temperature sensor file.
 */
void init_coolant_sensor_path(void);

/**
 * @brief Read the current coolant temperature.
 *
 * @return The coolant temperature in degrees Celsius, or a negative value on error.
 */
float read_coolant_temp(void);

/**
 * @brief Path to the coolant temperature sensor file (set by init_coolant_sensor_path).
 */
extern char coolant_temp_path[512];

#endif // COOLANT_MONITOR_H
