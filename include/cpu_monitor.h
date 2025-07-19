/**
 * @file cpu_monitor.h
 * @brief CPU temperature and usage monitoring interface for CoolerDash.
 *
 * Coding Standards (C99, ISO/IEC 9899:1999):
 * - All code comments in English.
 * - Doxygen-style comments for all functions (description, @brief, @param, @return, examples).
 * - Opening braces on the same line (K&R style).
 * - Always check return values of malloc(), calloc(), realloc().
 * - Free all dynamically allocated memory and set pointers to NULL after freeing.
 * - Use include guards in all headers.
 * - Include only necessary headers, system headers before local headers.
 * - Function names are verbs, use snake_case for functions/variables, UPPER_CASE for macros, PascalCase for typedef structs.
 * - Use descriptive names, avoid abbreviations.
 * - Document complex algorithms and data structures.
 */
#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

// =============================================================================
// CPU MONITORING MODULE
// =============================================================================

/**
 * @brief Initialize the CPU temperature sensor path.
 *
 * Detects and sets the path to the CPU temperature sensor file.
 *
 * @return void
 *
 * Example:
 * @code
 * init_cpu_sensor_path();
 * @endcode
 */
void init_cpu_sensor_path(void);

/**
 * @brief Read the current CPU temperature.
 *
 * Reads the temperature from the CPU sensor file set by init_cpu_sensor_path().
 *
 * @return The CPU temperature in degrees Celsius, or 0.0f on error.
 *
 * Example:
 * @code
 * float temp = read_cpu_temp();
 * @endcode
 */
float read_cpu_temp(void);

// CPU Usage Functions (only for modes 1, 2, 3)
/**
 * @brief Structure to hold CPU statistics for usage calculation.
 *
 * Holds total and idle CPU time for usage calculations.
 *
 * Example:
 * @code
 * cpu_stat_t stat;
 * get_cpu_stat(&stat);
 * @endcode
 */
typedef struct {
    long total; /**< Total CPU time. */
    long idle;  /**< Idle CPU time. */
} cpu_stat_t;

/**
 * @brief Get the current CPU statistics.
 *
 * Reads the current CPU statistics from /proc/stat and fills the provided structure.
 *
 * @param[out] stat Pointer to a cpu_stat_t structure to be filled.
 * @return 1 on success, 0 on failure.
 *
 * Example:
 * @code
 * cpu_stat_t stat;
 * if (get_cpu_stat(&stat)) {
 *     // use stat
 * }
 * @endcode
 */
int get_cpu_stat(cpu_stat_t *stat);

/**
 * @brief Calculate CPU usage percentage between two samples.
 *
 * Calculates the CPU usage as a percentage between two cpu_stat_t samples.
 *
 * @param last_stat Pointer to the previous CPU statistics.
 * @param curr_stat Pointer to the current CPU statistics.
 * @return CPU usage as a percentage (0.0 - 100.0), -1.0f on error.
 *
 * Example:
 * @code
 * float usage = calculate_cpu_usage(&last, &curr);
 * @endcode
 */
float calculate_cpu_usage(const cpu_stat_t *last_stat, const cpu_stat_t *curr_stat);

// RAM Usage Functions (only for modes 1, 2, 3)
/**
 * @brief Get the current RAM usage as a percentage.
 *
 * Reads the current RAM usage from /proc/meminfo and returns the usage as a percentage.
 *
 * @return RAM usage as a percentage (0.0 - 100.0), -1.0f on error.
 *
 * Example:
 * @code
 * float ram = get_ram_usage();
 * @endcode
 */
float get_ram_usage(void);

/**
 * @brief Path to the CPU temperature sensor file (set by init_cpu_sensor_path).
 */
extern char cpu_temp_path[512];

#endif // CPU_MONITOR_H
