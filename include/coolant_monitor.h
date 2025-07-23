/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file coolant_monitor.h
 * @brief Coolant temperature monitoring interface for CoolerDash.
 *
 * @details
 * Provides functions and global variables for reading coolant temperature from system sensors.
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
 * @todo Add support for additional coolant sensor types if required.
 * @example See function documentation for usage examples.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
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
