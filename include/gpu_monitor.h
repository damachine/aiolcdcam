/**
 * @file gpu_monitor.h
 * @brief GPU temperature and usage monitoring interface for CoolerDash.
 */
#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

/**
 * @brief Structure to hold GPU monitoring data.
 *
 * Holds temperature, usage, and memory usage values for the GPU.
 *
 * Example:
 * @code
 * gpu_data_t data;
 * get_gpu_data_full(&data);
 * @endcode
 */
typedef struct {
    float temperature;   /**< GPU temperature in degrees Celsius. */
    float usage;         /**< GPU usage as a percentage (0.0 - 100.0). */
    float memory_usage;  /**< GPU memory usage as a percentage (0.0 - 100.0). */
} gpu_data_t;

/**
 * @brief Initialize the GPU monitoring subsystem.
 *
 * Initializes the GPU monitoring backend and checks for GPU availability.
 *
 * @return 1 if GPU available, 0 if not
 *
 * Example:
 * @code
 * if (init_gpu_monitor()) {
 *     // GPU available
 * }
 * @endcode
 */
int init_gpu_monitor(void);

/**
 * @brief Read the current GPU temperature.
 *
 * Reads the current temperature from the GPU sensor.
 *
 * @return GPU temperature in degrees Celsius, 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_gpu_temp();
 * @endcode
 */
float read_gpu_temp(void);

/**
 * @brief Get the current GPU usage and memory usage.
 *
 * Reads the current GPU usage and memory usage percentages.
 *
 * @param[out] usage Pointer to store the GPU usage percentage
 * @param[out] mem_usage Pointer to store the GPU memory usage percentage
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * float usage, mem;
 * if (get_gpu_usage_data(&usage, &mem)) {
 *     // use usage and mem
 * }
 * @endcode
 */
int get_gpu_usage_data(float *usage, float *mem_usage);

/**
 * @brief Get all available GPU monitoring data.
 *
 * Fills a gpu_data_t structure with temperature, usage, and memory usage values.
 *
 * @param[out] data Pointer to a gpu_data_t structure to be filled
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * gpu_data_t data;
 * if (get_gpu_data_full(&data)) {
 *     // use data
 * }
 * @endcode
 */
int get_gpu_data_full(gpu_data_t *data);

#endif // GPU_MONITOR_H
