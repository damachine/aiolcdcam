#include "../include/gpu_monitor.h"
#include "../include/config.h"
#include <stdio.h>
#include <time.h>

// Global variable for GPU availability
static int gpu_available = -1;  // -1 = unknown, 0 = not available, 1 = available

/**
 * Checks GPU availability and initializes GPU monitoring.
 *
 * @return 1 if GPU available, 0 if not
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
 * Reads only GPU temperature (optimized for mode "def").
 *
 * @return GPU temperature in degrees Celsius (float), 0.0f on error
 */
float read_gpu_temp(void) {
    if (!init_gpu_monitor()) return 0.0f;  // GPU not available
    
    static time_t last_update = 0;
    static float cached_temp = 0;
    time_t now = time(NULL);
    
    if (now - last_update >= GPU_CACHE_INTERVAL) {
        FILE *fp = popen("nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            if (fscanf(fp, "%f", &cached_temp) != 1) {
                cached_temp = 0.0f;
            }
            pclose(fp);
            last_update = now;
        }
    }
    return cached_temp;
}

/**
 * Reads GPU usage data (for modes 1, 2, 3).
 *
 * @param[out] usage GPU utilization in percent
 * @param[out] mem_usage GPU RAM usage in percent
 * @return 1 on success, 0 on error
 */
int get_gpu_usage_data(float *usage, float *mem_usage) {
    if (!init_gpu_monitor() || !usage || !mem_usage) {
        if (usage) *usage = 0.0f;
        if (mem_usage) *mem_usage = 0.0f;
        return 0;
    }
    
    static time_t last_update = 0;
    static float cached_usage = 0, cached_mem_usage = 0;
    time_t now = time(NULL);
    
    if (now - last_update >= GPU_CACHE_INTERVAL) {
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
            last_update = now;
        }
    }
    
    *usage = cached_usage;
    *mem_usage = cached_mem_usage;
    return 1;
}

/**
 * Reads all GPU data (temperature, usage, RAM usage) with caching.
 *
 * @param[out] data Pointer to GPU data structure
 * @return 1 on success, 0 on error
 */
int get_gpu_data_full(gpu_data_t *data) {
    static time_t last_update = 0;
    static gpu_data_t cached_data = {0};
    time_t now = time(NULL);
    
    if (!data) return 0;
    
    if (now - last_update >= GPU_CACHE_INTERVAL) {
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
            last_update = now;
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
