#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

// =============================================================================
// GPU MONITORING MODULE
// =============================================================================

// GPU Data Structure
typedef struct {
    float temperature;
    float usage;
    float memory_usage;
} gpu_data_t;

// GPU Functions
int init_gpu_monitor(void);  // Initialization
float read_gpu_temp(void);   // Temperature only (for mode "def")
int get_gpu_usage_data(float *usage, float *mem_usage);  // Usage data (for modes 1, 2, 3)
int get_gpu_data_full(gpu_data_t *data);  // All data (complete)

#endif // GPU_MONITOR_H
