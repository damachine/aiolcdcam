/*
 * @note Must be initialized before calling read_cpu_temp().
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */
/**
 * @file gpu_monitor.h
 * @brief GPU temperature monitoring interface for CoolerDash.
 *
 * @details
 * Provides functions and data structures for reading GPU temperature from system sensors.
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
 * @todo Add support for additional GPU sensor types if required.
 * @example See function documentation for usage examples.
 * @author damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include "config.h"

/**
 * @brief Structure to hold GPU monitoring data.
 *
 * This struct is used to aggregate all relevant GPU monitoring values (temperature, usage, memory usage).
 *
 * @note All values are in degrees Celsius or percent (0.0 - 100.0).
 *
 * Example:
 * @code
 * gpu_data_t data;
 * get_gpu_data_full(&config, &data);
 * @endcode
 */
typedef struct {
    float temperature;   /**< GPU temperature in degrees Celsius. */
} gpu_data_t;

/**
 * @brief Initialize the GPU monitoring subsystem using configuration.
 *
 * Initializes the GPU monitoring backend and checks for GPU availability using config values.
 *
 * @param config Pointer to configuration struct (Config).
 * @return 1 if GPU available, 0 if not
 *
 * Example:
 * @code
 * if (init_gpu_monitor(&config)) {
 *     // GPU available
 * }
 * @endcode
 */
int init_gpu_monitor(const Config *config);

/**
 * @brief Read the current GPU temperature using configuration.
 *
 * Reads the current temperature from the GPU sensor using config values.
 *
 * @param config Pointer to configuration struct (Config).
 * @return GPU temperature in degrees Celsius, 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_gpu_temp(&config);
 * @endcode
 */
float read_gpu_temp(const Config *config);

/**
 * @brief Get GPU monitoring data.
 *
 * Fills a gpu_data_t structure with temperature value.
 *
 * @param config Pointer to configuration struct (Config).
 * @param[out] data Pointer to a gpu_data_t structure to be filled
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * gpu_data_t data;
 * if (get_gpu_data_full(&config, &data)) {
 *     // use data
 * }
 * @endcode
 */
int get_gpu_data_full(const Config *config, gpu_data_t *data);

#endif // GPU_MONITOR_H
