/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file cpu_monitor.h
 * @brief CPU temperature monitoring interface for CoolerDash.
 *
 * @details
 * Provides functions and global variables for reading CPU temperature from system sensors.
 *
 * Coding and Documentation Standards for CoolerDash:
 * - All code comments are written in English.
 * - Doxygen style is used for all function comments.
 * - See coding standards in project documentation and config.h for details.
 * - Opening braces for functions and control structures are placed on the same line (K&R style).
 * - Only necessary headers are included; system and local headers are separated.
 * - Code is indented with 4 spaces, no tabs.
 * - All functions, variables, and types follow project naming conventions (snake_case, PascalCase, UPPER_CASE).
 * - Inline comments are used sparingly and only when necessary.
 * - Redundant comments are avoided.
 * - All dynamically allocated memory is freed and pointers set to NULL.
 * - All malloc/calloc/realloc return values are checked.
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
 * @warning This file must comply with ISO/IEC 9899:1999 (C99).
 * @see config.h
 * @todo Add support for additional sensor types if required.
 * @example See function documentation for usage examples.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include "config.h"

/**
 * @brief Initialize the CPU temperature sensor path using configuration.
 *
 * @details
 * Detects and sets the path to the CPU temperature sensor file using the hwmon_path from Config.
 * This function must be called before any temperature readings are performed.
 *
 * @param[in] config Pointer to configuration struct (Config).
 * @pre config must not be NULL.
 * @post cpu_temp_path is set to the detected sensor file path.
 * @return void
 * @note Only supports Linux hwmon sensors in current implementation.
 * @warning If the sensor path is not found, temperature readings will fail.
 * @see read_cpu_temp
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 * @example
 *     init_cpu_sensor_path(&config);
 * @todo Add support for additional sensor types and error reporting.
 */
void init_cpu_sensor_path(const Config *config);

/**
 * @brief Read the current CPU temperature.
 *
 * @details
 * Reads the temperature from the CPU sensor file set by init_cpu_sensor_path().
 * Returns the temperature in degrees Celsius, or 0.0f on error.
 *
 * @pre cpu_temp_path must be set to a valid sensor file path.
 * @post Returns the current CPU temperature or 0.0f on error.
 * @return The CPU temperature in degrees Celsius, or 0.0f on error.
 * @note This function does not perform error reporting, only returns 0.0f on failure.
 * @warning If the sensor file is missing or unreadable, the returned value will be 0.0f.
 * @see init_cpu_sensor_path
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 * @example
 *     float temp = read_cpu_temp();
 * @todo Add error reporting and support for additional sensor types.
 */
float read_cpu_temp(void);

/**
 * @brief Path to the CPU temperature sensor file (set by init_cpu_sensor_path).
 *
 * @details
 * This global variable holds the path to the CPU temperature sensor file, which is detected and set during initialization.
 * It is used by read_cpu_temp() to access the correct sensor file for temperature readings.
 *
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
extern char cpu_temp_path[512];

#endif // CPU_MONITOR_H
