/**
 * @file cpu_monitor.c
 * @brief CPU temperature and usage monitoring implementation for CoolerDash.
 */
#include "../include/cpu_monitor.h"
#include "../include/config.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

// Global variable for cached CPU temperature path
char cpu_temp_path[512] = {0};

/**
 * @brief Initialize hwmon sensor path for CPU temperature at startup (once).
 *
 * Detects and sets the path to the CPU temperature sensor file by scanning available hwmon entries.
 *
 * @return void
 *
 * Example:
 * @code
 * init_cpu_sensor_path();
 * @endcode
 */
void init_cpu_sensor_path(void) {
    DIR *dir = opendir(HWMON_PATH);
    if (!dir) return;
    
    struct dirent *entry;
    char label_path[512], label[64];
    
    // Scan through hwmon directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files/directories
        
        // Check each possible temp label for the current hwmon entry
        for (int i = 1; i <= 9; ++i) {
            snprintf(label_path, sizeof(label_path), HWMON_PATH"/%s/temp%d_label", entry->d_name, i);
            FILE *flabel = fopen(label_path, "r");
            if (!flabel) continue;
            
            // Read the label and check if it matches "Package id 0"
            if (fgets(label, sizeof(label), flabel)) {
                // Cache CPU temperature path for later use
                if (strstr(label, "Package id 0") && strlen(cpu_temp_path) == 0) {
                    snprintf(cpu_temp_path, sizeof(cpu_temp_path), HWMON_PATH"/%s/temp%d_input", entry->d_name, i);
                    fclose(flabel);
                    closedir(dir);
                    return;
                }
            }
            fclose(flabel);
        }
    }
    closedir(dir);
}

/**
 * @brief Read CPU temperature from cached hwmon path.
 *
 * Reads the temperature from the CPU sensor file set by init_cpu_sensor_path().
 *
 * @return Temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_cpu_temp();
 * @endcode
 */
float read_cpu_temp(void) {
    if (strlen(cpu_temp_path) == 0) return 0.0f;
    
    FILE *finput = fopen(cpu_temp_path, "r");
    if (!finput) return 0.0f;
    
    int t = 0;
    float temp = 0.0f;
    // Read the integer temperature value, convert to float
    if (fscanf(finput, "%d", &t) == 1) {
        temp = t > 200 ? t / 1000.0f : (float)t;
    }
    fclose(finput);
    return temp;
}

/**
 * @brief Read CPU statistics for usage calculation.
 *
 * Reads the current CPU statistics from /proc/stat and fills the provided structure.
 *
 * @param[out] stat CPU statistics structure
 * @return 1 on success, 0 on error
 *
 * Example:
 * @code
 * cpu_stat_t stat;
 * if (get_cpu_stat(&stat)) {
 *     // use stat
 * }
 * @endcode
 */
int get_cpu_stat(cpu_stat_t *stat) {
    FILE *fstat = fopen("/proc/stat", "r");
    if (!fstat) { 
        stat->total = 0; 
        stat->idle = 0; 
        return 0; 
    }
    
    long user, nice, system, idle_val, iowait, irq, softirq, steal;
    const int result = fscanf(fstat, "cpu %ld %ld %ld %ld %ld %ld %ld %ld", 
                       &user, &nice, &system, &idle_val, &iowait, &irq, &softirq, &steal);
    fclose(fstat);
    
    if (result == 8) {
        // Calculate total and idle time for CPU
        stat->idle = idle_val + iowait;
        stat->total = stat->idle + user + nice + system + irq + softirq + steal;
        return 1;
    }
    
    stat->total = 0;
    stat->idle = 0;
    return 0;
}

/**
 * @brief Calculate CPU usage between two time points.
 *
 * Calculates the CPU usage as a percentage between two cpu_stat_t samples.
 *
 * @param last_stat Previous CPU statistics
 * @param curr_stat Current CPU statistics
 * @return CPU usage in percent (float), -1.0f on error
 *
 * Example:
 * @code
 * float usage = calculate_cpu_usage(&last, &curr);
 * @endcode
 */
float calculate_cpu_usage(const cpu_stat_t *last_stat, const cpu_stat_t *curr_stat) {
    if (!last_stat || !curr_stat) return -1.0f;
    
    const long totald = curr_stat->total - last_stat->total;
    const long idled = curr_stat->idle - last_stat->idle;
    
    if (totald <= 0) return -1.0f;  // Error indicator
    
    const float usage = 100.0f * (float)(totald - idled) / (float)totald;
    return (usage >= 0.0f && usage <= 100.0f) ? usage : 0.0f;
}

/**
 * @brief Read RAM usage from /proc/meminfo.
 *
 * Reads the current RAM usage from /proc/meminfo and returns the usage as a percentage.
 *
 * @return RAM usage in percent (float), -1.0f on error
 *
 * Example:
 * @code
 * float ram = get_ram_usage();
 * @endcode
 */
float get_ram_usage(void) {
    FILE *fmem = fopen("/proc/meminfo", "r");
    if (!fmem) return -1.0f;
    
    long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    char line[128];
    int found = 0;
    
    // Read and parse relevant lines from /proc/meminfo
    while (fgets(line, sizeof(line), fmem) && found < 4) {
        if (sscanf(line, "MemTotal: %ld", &mem_total) == 1) found++;
        else if (sscanf(line, "MemFree: %ld", &mem_free) == 1) found++;
        else if (sscanf(line, "Buffers: %ld", &buffers) == 1) found++;
        else if (sscanf(line, "Cached: %ld", &cached) == 1) found++;
    }
    fclose(fmem);
    
    if (found == 4 && mem_total > 0) {
        const long mem_used = mem_total - (mem_free + buffers + cached);
        if (mem_used >= 0) {
            return 100.0f * (float)mem_used / (float)mem_total;
        }
    }
    return -1.0f;
}
