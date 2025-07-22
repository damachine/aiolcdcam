/**
 * @file coolant_monitor.c
 * @brief Coolant temperature monitoring implementation for CoolerDash.
 */

// Include necessary headers
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

// Include project headers
#include "../include/coolant_monitor.h"
#include "../include/config.h"

// Global variable for cached coolant temperature path
char coolant_temp_path[512] = {0};

/**
 * @brief Initializes the hwmon sensor path for coolant temperature at startup (once).
 *
 * Detects and sets the path to the coolant temperature sensor file by scanning available hwmon entries.
 *
 * @return void
 *
 * Example:
 * @code
 * init_coolant_sensor_path();
 * @endcode
 */
void init_coolant_sensor_path(void) {
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
                // Cache coolant temperature path
                if ((strstr(label, "Coolant") || strstr(label, "coolant")) && strlen(coolant_temp_path) == 0) {
                    snprintf(coolant_temp_path, sizeof(coolant_temp_path), HWMON_PATH"/%s/temp%d_input", entry->d_name, i);
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
 * @brief Reads coolant temperature from cached hwmon path.
 *
 * Reads the temperature from the coolant sensor file set by init_coolant_sensor_path().
 *
 * @return Temperature in degrees Celsius (float), 0.0f on error
 *
 * Example:
 * @code
 * float temp = read_coolant_temp();
 * @endcode
 */
float read_coolant_temp(void) {
    if (strlen(coolant_temp_path) == 0) return 0.0f;
    
    FILE *finput = fopen(coolant_temp_path, "r");
    if (!finput) return 0.0f;
    
    int t = 0;
    float temp = 0.0f;
    if (fscanf(finput, "%d", &t) == 1) {
        temp = t > 200 ? t / 1000.0f : (float)t;
    }
    fclose(finput);
    return temp;
}
