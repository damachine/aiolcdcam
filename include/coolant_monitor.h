/**
 * @file coolant_monitor.h
 * @brief Coolant temperature monitoring interface for CoolerDash.
 */

#ifndef COOLANT_MONITOR_H
#define COOLANT_MONITOR_H

#include "config.h"

/**
 * @brief Initialize the coolant temperature sensor path using configuration.
 *
 * Detects and sets the path to the coolant temperature sensor file using the hwmon_path from Config.
 *
 * @param config Pointer to configuration struct (Config).
 * @return void
 *
 * Example:
 * @code
 * init_coolant_sensor_path(&config);
 * @endcode
 */
void init_coolant_sensor_path(const Config *config);

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
 *
 * This global variable holds the path to the coolant temperature sensor file, which is detected and set during initialization.
 * It is used by read_coolant_temp() to access the correct sensor file for temperature readings.
 */
extern char coolant_temp_path[512];

#endif // COOLANT_MONITOR_H
