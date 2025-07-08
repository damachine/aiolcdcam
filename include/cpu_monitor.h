#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

// =============================================================================
// CPU MONITORING MODULE
// =============================================================================

// CPU Temperature Functions
void init_cpu_sensor_path(void);
float read_cpu_temp(void);

// CPU Usage Functions (only for modes 1, 2, 3)
typedef struct {
    long total;
    long idle;
} cpu_stat_t;

int get_cpu_stat(cpu_stat_t *stat);
float calculate_cpu_usage(const cpu_stat_t *last_stat, const cpu_stat_t *curr_stat);

// RAM Usage Functions (only for modes 1, 2, 3)
float get_ram_usage(void);

// Global CPU path variable (extern, defined in cpu_monitor.c)
extern char cpu_temp_path[512];

#endif // CPU_MONITOR_H
