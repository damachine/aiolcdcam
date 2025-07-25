/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/**
 * @brief CPU temperature monitoring interface for CoolerDash.
 * @details Provides functions and global variables for reading CPU temperature from system sensors.
 * @example
 *     See function documentation for usage examples.
 */

// Function prototypes
#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

// Include project headers
#include "config.h"

/**
 * @brief Initialize the CPU temperature sensor path using configuration.
 * @details Detects and sets the path to the CPU temperature sensor file using the hwmon_path from Config. This function must be called before any temperature readings are performed. Internally uses opendir(), readdir(), fopen(), fclose(), closedir(). Always checks and frees resources. The path is cached in cpu_temp_path.
 * @example
 *     init_cpu_sensor_path(&config);
 */
void init_cpu_sensor_path(const Config *config);

/**
 * @brief Read the current CPU temperature.
 * @details Reads the temperature from the CPU sensor file set by init_cpu_sensor_path(). Uses fopen(), fscanf(), fclose(). Always checks and frees resources. Returns the temperature in degrees Celsius, or 0.0f on error or if not initialized.
 * @example
 *     float temp = read_cpu_temp();
 */
float read_cpu_temp(void);

/**
 * @brief Path to the CPU temperature sensor file (set by init_cpu_sensor_path).
 * @details This global variable holds the path to the CPU temperature sensor file, which is detected and set during initialization. It is used by read_cpu_temp() to access the correct sensor file for temperature readings. Must be initialized before calling read_cpu_temp().
 * @example
 *     // Must be initialized before calling read_cpu_temp().
 */
extern char cpu_temp_path[512];

#endif // CPU_MONITOR_H
