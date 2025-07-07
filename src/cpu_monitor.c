#include "cpu_monitor.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

// Globale Variable für gecachten CPU-Temperatur-Pfad
char cpu_temp_path[512] = {0};

/**
 * Initialisiert den hwmon-Sensor-Pfad für CPU-Temperatur beim Start (einmalig)
 */
void init_cpu_sensor_path(void) {
    DIR *dir = opendir(HWMON_PATH);
    if (!dir) return;
    
    struct dirent *entry;
    char label_path[512], label[64];
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        for (int i = 1; i <= 9; ++i) {
            snprintf(label_path, sizeof(label_path), HWMON_PATH"/%s/temp%d_label", entry->d_name, i);
            FILE *flabel = fopen(label_path, "r");
            if (!flabel) continue;
            
            if (fgets(label, sizeof(label), flabel)) {
                // CPU-Temperatur Pfad cachen
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
 * Liest die CPU-Temperatur aus dem gecachten hwmon-Pfad.
 *
 * @return Temperatur in Grad Celsius (float), 0.0f bei Fehler
 */
float read_cpu_temp(void) {
    if (strlen(cpu_temp_path) == 0) return 0.0f;
    
    FILE *finput = fopen(cpu_temp_path, "r");
    if (!finput) return 0.0f;
    
    int t = 0;
    float temp = 0.0f;
    if (fscanf(finput, "%d", &t) == 1) {
        temp = t > 200 ? t / 1000.0f : (float)t;
    }
    fclose(finput);
    return temp;
}

/**
 * Liest CPU-Statistiken für die Auslastungsberechnung.
 *
 * @param[out] stat CPU-Statistik-Struktur
 * @return 1 bei Erfolg, 0 bei Fehler
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
        stat->idle = idle_val + iowait;
        stat->total = stat->idle + user + nice + system + irq + softirq + steal;
        return 1;
    }
    
    stat->total = 0;
    stat->idle = 0;
    return 0;
}

/**
 * Berechnet die CPU-Auslastung zwischen zwei Zeitpunkten.
 *
 * @param last_stat Vorherige CPU-Statistik
 * @param curr_stat Aktuelle CPU-Statistik
 * @return CPU-Auslastung in Prozent (float), -1.0f bei Fehler
 */
float calculate_cpu_usage(const cpu_stat_t *last_stat, const cpu_stat_t *curr_stat) {
    if (!last_stat || !curr_stat) return -1.0f;
    
    const long totald = curr_stat->total - last_stat->total;
    const long idled = curr_stat->idle - last_stat->idle;
    
    if (totald <= 0) return -1.0f;  // Fehler-Indikator
    
    const float usage = 100.0f * (float)(totald - idled) / (float)totald;
    return (usage >= 0.0f && usage <= 100.0f) ? usage : 0.0f;
}

/**
 * Liest die RAM-Auslastung aus /proc/meminfo.
 *
 * @return RAM-Auslastung in Prozent (float), -1.0f bei Fehler
 */
float get_ram_usage(void) {
    FILE *fmem = fopen("/proc/meminfo", "r");
    if (!fmem) return -1.0f;
    
    long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    char line[128];
    int found = 0;
    
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
