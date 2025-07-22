/**
 * @file gpu_monitor.c
 * @brief GPU temperature and usage monitoring implementation for CoolerDash.
 */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// Include project headers
#include "../include/gpu_monitor.h"
#include "../include/config.h"

// Global variable for GPU availability
static int gpu_available = -1;  // -1 = unknown, 0 = not available, 1 = available

/**
 * @brief Get current time in milliseconds.
 *
 * Returns the current system time in milliseconds since the epoch.
 *
 * @return Current time in milliseconds
 *
 * Example:
 * @code
 * long long now = get_current_time_ms();
 * @endcode
 */
static long long get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (long long)(tv.tv_usec) / 1000;
}

/**
 * @brief Checks GPU availability and initializes GPU monitoring.
 *
 * Checks if an NVIDIA GPU is available and initializes the monitoring backend.
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
int init_gpu_monitor(void) {
    if (gpu_available != -1) {
        return gpu_available;  // Already checked
    }
    
    FILE *fp = popen("nvidia-smi -L 2>/dev/null", "r");
    if (fp) {
        char line[256];
        if (fgets(line, sizeof(line), fp) != NULL) {
            gpu_available = 1;  // GPU found
        } else {
            gpu_available = 0;  // No GPU found
        }
        pclose(fp);
    } else {
        gpu_available = 0;  // nvidia-smi not available
    }
    
    return gpu_available;
}

/**
 * @brief Reads only GPU temperature (optimized for mode "def").
 *
 * Reads the current temperature from the GPU sensor, with caching for performance.
 *
 * @return GPU temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_gpu_temp();
 * @endcode
 */
float read_gpu_temp(void) {
    if (!init_gpu_monitor()) return 0.0f;  // GPU not available
    
    static long long last_update_ms = 0;
    static float cached_temp = 0;
    long long now_ms = get_current_time_ms();
    long long cache_interval_ms = (long long)(GPU_CACHE_INTERVAL * 1000);
    
    if (now_ms - last_update_ms >= cache_interval_ms) {
        FILE *fp = popen("nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            if (fscanf(fp, "%f", &cached_temp) != 1) {
                cached_temp = 0.0f;
            }
            pclose(fp);
            last_update_ms = now_ms;
        }
    }
    return cached_temp;
}

/**
 * @brief Reads GPU usage data (for modes 1, 2, 3).
 *
 * Reads the current GPU usage and memory usage percentages, with caching for performance.
 *
 * @param[out] usage GPU utilization in percent
 * @param[out] mem_usage GPU RAM usage in percent
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
int get_gpu_usage_data(float *usage, float *mem_usage) {
    if (!init_gpu_monitor() || !usage || !mem_usage) {
        if (usage) *usage = 0.0f;
        if (mem_usage) *mem_usage = 0.0f;
        return 0;
    }
    
    static long long last_update_ms = 0;
    static float cached_usage = 0, cached_mem_usage = 0;
    long long now_ms = get_current_time_ms();
    long long cache_interval_ms = (long long)(GPU_CACHE_INTERVAL * 1000);
    
    if (now_ms - last_update_ms >= cache_interval_ms) {
        FILE *fp = popen("nvidia-smi --query-gpu=utilization.gpu,memory.used,memory.total --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            float mem_total = 0, mem_used = 0;
            if (fscanf(fp, "%f, %f, %f", &cached_usage, &mem_used, &mem_total) == 3) {
                cached_mem_usage = (mem_total > 0) ? (100.0f * mem_used / mem_total) : 0.0f;
            } else {
                cached_usage = 0.0f;
                cached_mem_usage = 0.0f;
            }
            pclose(fp);
            last_update_ms = now_ms;
        }
    }
    
    *usage = cached_usage;
    *mem_usage = cached_mem_usage;
    return 1;
}

/**
 * @brief Reads all GPU data (temperature, usage, RAM usage) with caching.
 *
 * Fills a gpu_data_t structure with temperature, usage, and memory usage values.
 *
 * @param[out] data Pointer to GPU data structure
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
int get_gpu_data_full(gpu_data_t *data) {
    static long long last_update_ms = 0;
    static gpu_data_t cached_data = {0};
    long long now_ms = get_current_time_ms();
    long long cache_interval_ms = (long long)(GPU_CACHE_INTERVAL * 1000);
    
    if (!data) return 0;
    
    if (now_ms - last_update_ms >= cache_interval_ms) {
        FILE *fp = popen("nvidia-smi --query-gpu=temperature.gpu,utilization.gpu,memory.used,memory.total --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            float mem_total = 0, mem_used = 0;
            if (fscanf(fp, "%f, %f, %f, %f", &cached_data.temperature, &cached_data.usage, &mem_used, &mem_total) == 4) {
                cached_data.memory_usage = (mem_total > 0) ? (100.0f * mem_used / mem_total) : 0.0f;
            } else {
                // Error reading - set to 0
                cached_data.temperature = 0.0f;
                cached_data.usage = 0.0f;
                cached_data.memory_usage = 0.0f;
            }
            pclose(fp);
            last_update_ms = now_ms;
        } else {
            // nvidia-smi not available
            cached_data.temperature = 0.0f;
            cached_data.usage = 0.0f;
            cached_data.memory_usage = 0.0f;
        }
    }
    
    *data = cached_data;
    return 1;
}
