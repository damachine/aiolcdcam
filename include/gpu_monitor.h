#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

// =============================================================================
// GPU MONITORING MODULE
// =============================================================================

// GPU Data Structure
/**
 * @brief Structure to hold GPU monitoring data.
 */
typedef struct {
    float temperature;   /**< GPU temperature in degrees Celsius. */
    float usage;         /**< GPU usage as a percentage (0.0 - 100.0). */
    float memory_usage;  /**< GPU memory usage as a percentage (0.0 - 100.0). */
} gpu_data_t;

/**
 * @brief Initialize the GPU monitoring subsystem.
 *
 * @return 0 on success, -1 on failure.
 */
int init_gpu_monitor(void);

/**
 * @brief Read the current GPU temperature.
 *
 * @return GPU temperature in degrees Celsius, or a negative value on error.
 */
float read_gpu_temp(void);

/**
 * @brief Get the current GPU usage and memory usage.
 *
 * @param usage Pointer to store the GPU usage percentage.
 * @param mem_usage Pointer to store the GPU memory usage percentage.
 * @return 0 on success, -1 on failure.
 */
int get_gpu_usage_data(float *usage, float *mem_usage);

/**
 * @brief Get all available GPU monitoring data.
 *
 * @param data Pointer to a gpu_data_t structure to be filled.
 * @return 0 on success, -1 on failure.
 */
int get_gpu_data_full(gpu_data_t *data);

#endif // GPU_MONITOR_H
