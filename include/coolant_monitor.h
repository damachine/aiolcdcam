#ifndef COOLANT_MONITOR_H
#define COOLANT_MONITOR_H

// =============================================================================
// COOLANT TEMPERATURE MONITORING MODULE
// =============================================================================

/**
 * @brief Initialize the coolant temperature sensor path.
 *
 * Detects and sets the path to the coolant temperature sensor file.
 *
 * @return void
 *
 * Example:
 * @code
 * init_coolant_sensor_path();
 * @endcode
 */
void init_coolant_sensor_path(void);

/**
 * @brief Read the current coolant temperature.
 *
 * Reads the temperature from the coolant sensor file set by init_coolant_sensor_path().
 *
 * @return The coolant temperature in degrees Celsius, or 0.0f on error.
 *
 * Example:
 * @code
 * float temp = read_coolant_temp();
 * @endcode
 */
float read_coolant_temp(void);

/**
 * @brief Path to the coolant temperature sensor file (set by init_coolant_sensor_path).
 */
extern char coolant_temp_path[512];

#endif // COOLANT_MONITOR_H
