#ifndef COOLANT_MONITOR_H
#define COOLANT_MONITOR_H

// =============================================================================
// COOLANT TEMPERATURE MONITORING MODULE
// =============================================================================

// Coolant Temperature Functions
void init_coolant_sensor_path(void);
float read_coolant_temp(void);

// Globale Coolant-Pfad-Variable (extern, definiert in coolant_monitor.c)
extern char coolant_temp_path[512];

#endif // COOLANT_MONITOR_H
