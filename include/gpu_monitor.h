/**
 * @file gpu_monitor.h
 * @brief GPU temperature and usage monitoring interface for CoolerDash.
 */
#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include "config.h"

/**
 * @brief Structure to hold GPU monitoring data.
 *
 * This struct is used to aggregate all relevant GPU monitoring values (temperature, usage, memory usage).
 * It is filled by get_gpu_data_full() and can be extended for future metrics.
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
    float usage;         /**< GPU usage as a percentage (0.0 - 100.0). */
    float memory_usage;  /**< GPU memory usage as a percentage (0.0 - 100.0). */
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
 * @brief Get the current GPU usage and memory usage using configuration.
 *
 * Reads the current GPU usage and memory usage percentages using config values.
 *
 * @param config Pointer to configuration struct (Config).
 * @param[out] usage Pointer to store the GPU usage percentage
 * @param[out] mem_usage Pointer to store the GPU memory usage percentage
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * float usage, mem;
 * if (get_gpu_usage_data(&config, &usage, &mem)) {
 *     // use usage and mem
 * }
 * @endcode
 */
int get_gpu_usage_data(const Config *config, float *usage, float *mem_usage);

/**
 * @brief Get all available GPU monitoring data using configuration.
 *
 * Fills a gpu_data_t structure with temperature, usage, and memory usage values using config values.
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
