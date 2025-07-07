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
int init_gpu_monitor(void);  // Initialisierung
float read_gpu_temp(void);   // Nur Temperatur (für mode "def")
int get_gpu_usage_data(float *usage, float *mem_usage);  // Auslastungsdaten (für Modi 1, 2, 3)
int get_gpu_data_full(gpu_data_t *data);  // Alle Daten (komplett)

#endif // GPU_MONITOR_H
