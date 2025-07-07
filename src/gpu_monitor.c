#include "gpu_monitor.h"
#include "config.h"
#include <stdio.h>
#include <time.h>

#include "gpu_monitor.h"
#include "config.h"
#include <stdio.h>
#include <time.h>

// Globale Variable für GPU-Verfügbarkeit
static int gpu_available = -1;  // -1 = unbekannt, 0 = nicht verfügbar, 1 = verfügbar

/**
 * Überprüft die GPU-Verfügbarkeit und initialisiert das GPU-Monitoring.
 *
 * @return 1 wenn GPU verfügbar, 0 wenn nicht
 */
int init_gpu_monitor(void) {
    if (gpu_available != -1) {
        return gpu_available;  // Bereits gecheckt
    }
    
    FILE *fp = popen("nvidia-smi -L 2>/dev/null", "r");
    if (fp) {
        char line[256];
        if (fgets(line, sizeof(line), fp) != NULL) {
            gpu_available = 1;  // GPU gefunden
        } else {
            gpu_available = 0;  // Keine GPU gefunden
        }
        pclose(fp);
    } else {
        gpu_available = 0;  // nvidia-smi nicht verfügbar
    }
    
    return gpu_available;
}

/**
 * Liest nur die GPU-Temperatur (optimiert für mode "def").
 *
 * @return GPU-Temperatur in Grad Celsius (float), 0.0f bei Fehler
 */
float read_gpu_temp(void) {
    if (!init_gpu_monitor()) return 0.0f;  // GPU nicht verfügbar
    
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
 * Liest GPU-Auslastungsdaten (für Modi 1, 2, 3).
 *
 * @param[out] usage GPU-Auslastung in Prozent
 * @param[out] mem_usage GPU-RAM-Auslastung in Prozent
 * @return 1 bei Erfolg, 0 bei Fehler
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
 * Liest alle GPU-Daten (Temperatur, Auslastung, RAM-Auslastung) mit Caching.
 *
 * @param[out] data Zeiger auf GPU-Daten-Struktur
 * @return 1 bei Erfolg, 0 bei Fehler
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
                // Fehler beim Lesen - setze auf 0
                cached_data.temperature = 0.0f;
                cached_data.usage = 0.0f;
                cached_data.memory_usage = 0.0f;
            }
            pclose(fp);
            last_update = now;
        } else {
            // nvidia-smi nicht verfügbar
            cached_data.temperature = 0.0f;
            cached_data.usage = 0.0f;
            cached_data.memory_usage = 0.0f;
        }
    }
    
    *data = cached_data;
    return 1;
}
