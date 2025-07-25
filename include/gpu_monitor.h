/*
 * @author damachine (christkue79@gmail.com)
 * @website https://github.com/damachine
 * @copyright (c) 2025 damachine
 * @license MIT
 * @version 1.0
 */

/** 
 * @brief GPU temperature monitoring interface for CoolerDash.
 * @details Provides functions and data structures for reading GPU temperature from system sensors.
 * @example
 *     See function documentation for usage examples.
 */

// Function prototypes
#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

// Include project headers
#include "config.h"

/**
 * @brief Structure to hold GPU monitoring data.
 * @details This struct is used to aggregate all relevant GPU monitoring values (temperature, usage, memory usage).
 * @example
 *     gpu_data_t data;
 *     get_gpu_data_full(&config, &data);
 */
typedef struct {
    float temperature; // GPU temperature in degrees Celsius.
} gpu_data_t;

/**
 * @brief Initialize the GPU monitoring subsystem using configuration.
 * @details Initializes the GPU monitoring backend and checks for GPU availability using config values.
 * @example
 *     if (init_gpu_monitor(&config)) {
 *         // GPU available
 *     }
 */
int init_gpu_monitor(const Config *config);

/**
 * @brief Read the current GPU temperature using configuration.
 * @details Reads the current temperature from the GPU sensor using config values.
 * @example
 *     float temp = read_gpu_temp(&config);
 */
float read_gpu_temp(const Config *config);

/**
 * @brief Get GPU monitoring data.
 * @details Fills a gpu_data_t structure with temperature value. Returns 1 on success, 0 on failure. Always check the return value and ensure the pointer is valid.
 * @example
 *     gpu_data_t data;
 *     if (get_gpu_data_full(&config, &data)) {
 *         // use data
 *     }
 */
int get_gpu_data_full(const Config *config, gpu_data_t *data);

#endif // GPU_MONITOR_H
