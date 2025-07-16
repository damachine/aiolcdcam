#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

// =============================================================================
// CPU MONITORING MODULE
// =============================================================================

// CPU Temperature Functions
/**
 * @brief Initialize the CPU temperature sensor path.
 *
 * This function detects and sets the path to the CPU temperature sensor file.
 */
void init_cpu_sensor_path(void);

/**
 * @brief Read the current CPU temperature.
 *
 * @return The CPU temperature in degrees Celsius, or a negative value on error.
 */
float read_cpu_temp(void);

// CPU Usage Functions (only for modes 1, 2, 3)
/**
 * @brief Structure to hold CPU statistics for usage calculation.
 */
typedef struct {
    long total; /**< Total CPU time. */
    long idle;  /**< Idle CPU time. */
} cpu_stat_t;

/**
 * @brief Get the current CPU statistics.
 *
 * @param stat Pointer to a cpu_stat_t structure to be filled.
 * @return 0 on success, -1 on failure.
 */
int get_cpu_stat(cpu_stat_t *stat);

/**
 * @brief Calculate CPU usage percentage between two samples.
 *
 * @param last_stat Pointer to the previous CPU statistics.
 * @param curr_stat Pointer to the current CPU statistics.
 * @return CPU usage as a percentage (0.0 - 100.0).
 */
float calculate_cpu_usage(const cpu_stat_t *last_stat, const cpu_stat_t *curr_stat);

// RAM Usage Functions (only for modes 1, 2, 3)
/**
 * @brief Get the current RAM usage as a percentage.
 *
 * @return RAM usage as a percentage (0.0 - 100.0).
 */
float get_ram_usage(void);

/**
 * @brief Path to the CPU temperature sensor file (set by init_cpu_sensor_path).
 */
extern char cpu_temp_path[512];

#endif // CPU_MONITOR_H
