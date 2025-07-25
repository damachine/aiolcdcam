/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/**
 * @brief Coolant temperature monitoring interface for CoolerDash.
 * @details Provides functions and global variables for reading coolant temperature from system sensors.
 * @example
 *     See function documentation for usage examples.
 */

// Function prototypes
#ifndef COOLANT_MONITOR_H
#define COOLANT_MONITOR_H

// Include project headers
#include "config.h"

/**
 * @brief Initialize the coolant temperature sensor path using configuration.
 * @details Detects and sets the path to the coolant temperature sensor file using the hwmon_path from Config.
 * @example
 *     init_coolant_sensor_path(&config);
 */
void init_coolant_sensor_path(const Config *config);

/**
 * @brief Read the current coolant temperature.
 * @details Reads the temperature from the coolant sensor file set by init_coolant_sensor_path().
 * @example
 *     float temp = read_coolant_temp();
 */
float read_coolant_temp(void);

// Path to the coolant temperature sensor file (set by init_coolant_sensor_path)
extern char coolant_temp_path[512];

#endif // COOLANT_MONITOR_H
