#include "../include/display.h"
#include "../include/config.h"
#include "../include/coolercontrol.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/coolant_monitor.h"
#include <cairo/cairo.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Konvertiert Mode-String zu Display-Mode-Enum
 *
 * @param mode_str Mode-String ("def", "1", "2", "3")
 * @return Display-Mode-Enum
 */
display_mode_t parse_display_mode(const char *mode_str) {
    if (!mode_str) return DISPLAY_MODE_DEF;
    
    if (strcmp(mode_str, "1") == 0) return DISPLAY_MODE_1;
    if (strcmp(mode_str, "2") == 0) return DISPLAY_MODE_2;
    if (strcmp(mode_str, "3") == 0) return DISPLAY_MODE_3;
    
    return DISPLAY_MODE_DEF;  // Default
}

/**
 * Berechnet Farbverlauf für Temperaturbalken (grün → orange → rot)
 *
 * @param val Temperaturwert
 * @param[out] r Rot-Komponente (0-255)
 * @param[out] g Grün-Komponente (0-255)
 * @param[out] b Blau-Komponente (0-255)
 */
void lerp_temp_color(float val, int* r, int* g, int* b) {
    if (val <= TEMP_THRESHOLD_GREEN) { 
        *r = COLOR_GREEN_R; *g = COLOR_GREEN_G; *b = COLOR_GREEN_B; 
    } else if (val <= TEMP_THRESHOLD_ORANGE) { 
        *r = COLOR_ORANGE_R; *g = COLOR_ORANGE_G; *b = COLOR_ORANGE_B; 
    } else if (val <= TEMP_THRESHOLD_RED) { 
        *r = COLOR_HOT_ORANGE_R; *g = COLOR_HOT_ORANGE_G; *b = COLOR_HOT_ORANGE_B; 
    } else { 
        *r = COLOR_RED_R; *g = COLOR_RED_G; *b = COLOR_RED_B; 
    }
}

// Forward-Deklarationen für interne Funktionen
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data);
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data, display_mode_t mode);
static void draw_mode_1_vertical_bars(cairo_t *cr, const sensor_data_t *data);
static void draw_mode_2_circles(cairo_t *cr, const sensor_data_t *data);
static void draw_mode_3_horizontal_bars(cairo_t *cr, const sensor_data_t *data);
static void draw_labels(cairo_t *cr, display_mode_t mode);
static int should_update_display(const sensor_data_t *data);

/**
 * Rendert das Display basierend auf Sensordaten und Modus
 *
 * @param data Zeiger auf Sensordaten
 * @param mode Display-Modus
 * @return 1 bei Erfolg, 0 bei Fehler
 */
int render_display(const sensor_data_t *data, display_mode_t mode) {
    if (!data) return 0;
    
    // Prüfe ob Update nötig ist
    if (!should_update_display(data)) {
        return 1; // Kein Update nötig, aber kein Fehler
    }
    
    // Cairo Surface erstellen
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!surface || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        if (surface) cairo_surface_destroy(surface);
        return 0;
    }
    
    cairo_t *cr = cairo_create(surface);
    if (!cr || cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        if (cr) cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return 0;
    }

    // Hintergrund schwarz
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Temperaturbalken zeichnen
    draw_temperature_bars(cr, data);
    
    // Labels zeichnen
    draw_labels(cr, mode);
    
    // Modus-spezifische Elemente zeichnen
    switch (mode) {
        case DISPLAY_MODE_1:
            draw_mode_1_vertical_bars(cr, data);
            break;
        case DISPLAY_MODE_2:
            draw_mode_2_circles(cr, data);
            break;
        case DISPLAY_MODE_3:
            draw_mode_3_horizontal_bars(cr, data);
            break;
        case DISPLAY_MODE_DEF:
        default:
            // Nur Temperaturen, keine zusätzlichen Elemente
            break;
    }
    
    // Temperatur-Displays zeichnen
    draw_temperature_displays(cr, data, mode);

    // Image-Directory erstellen falls nötig
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        mkdir(IMAGE_DIR, 0755);
    }
    
    // PNG speichern
    int success = 0;
    if (cairo_surface_write_to_png(surface, IMAGE_PATH) == CAIRO_STATUS_SUCCESS) {
        success = 1;
        
        // Bild 2x an das Display senden (behebt einen Fehler)
        if (is_session_initialized()) {
            for (int i = 0; i < 2; ++i) {
                send_image_to_lcd(IMAGE_PATH, KRAKEN_UID);
            }
        }
    }
    
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return success;
}

/**
 * Zeichnet die Temperaturbalken (CPU und GPU)
 */
static void draw_temperature_bars(cairo_t *cr, const sensor_data_t *data) {
    const int bar_x = (DISPLAY_WIDTH - BAR_WIDTH) / 2;
    const int cpu_bar_y = (DISPLAY_HEIGHT - (2 * BAR_HEIGHT + BAR_GAP)) / 2;
    const int gpu_bar_y = cpu_bar_y + BAR_HEIGHT + BAR_GAP;
    
    // CPU-Balken
    int r, g, b;
    lerp_temp_color(data->cpu_temp, &r, &g, &b);
    const int cpu_val_w = (data->cpu_temp > 0.0f) ? 
        (int)((data->cpu_temp / 100.0f) * BAR_WIDTH) : 0;
    const int safe_cpu_val_w = (cpu_val_w < 0) ? 0 : 
        (cpu_val_w > BAR_WIDTH) ? BAR_WIDTH : cpu_val_w;
    
    cairo_set_source_rgb(cr, COLOR_BG_R, COLOR_BG_G, COLOR_BG_B);
    cairo_rectangle(cr, bar_x, cpu_bar_y, BAR_WIDTH, BAR_HEIGHT);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    cairo_rectangle(cr, bar_x, cpu_bar_y, safe_cpu_val_w, BAR_HEIGHT);
    cairo_fill(cr);
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgb(cr, COLOR_BORDER_R, COLOR_BORDER_G, COLOR_BORDER_B);
    cairo_rectangle(cr, bar_x, cpu_bar_y, BAR_WIDTH, BAR_HEIGHT);
    cairo_stroke(cr);

    // GPU-Balken
    lerp_temp_color(data->gpu_temp, &r, &g, &b);
    const int gpu_val_w = (data->gpu_temp > 0.0f) ? 
        (int)((data->gpu_temp / 100.0f) * BAR_WIDTH) : 0;
    const int safe_gpu_val_w = (gpu_val_w < 0) ? 0 : 
        (gpu_val_w > BAR_WIDTH) ? BAR_WIDTH : gpu_val_w;
    
    cairo_set_source_rgb(cr, COLOR_BG_R, COLOR_BG_G, COLOR_BG_B);
    cairo_rectangle(cr, bar_x, gpu_bar_y, BAR_WIDTH, BAR_HEIGHT);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
    cairo_rectangle(cr, bar_x, gpu_bar_y, safe_gpu_val_w, BAR_HEIGHT);
    cairo_fill(cr);
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgb(cr, COLOR_BORDER_R, COLOR_BORDER_G, COLOR_BORDER_B);
    cairo_rectangle(cr, bar_x, gpu_bar_y, BAR_WIDTH, BAR_HEIGHT);
    cairo_stroke(cr);
}

/**
 * Zeichnet CPU/GPU Labels je nach Modus
 */
static void draw_labels(cairo_t *cr, display_mode_t mode) {
    const int cpu_bar_y = (DISPLAY_HEIGHT - (2 * BAR_HEIGHT + BAR_GAP)) / 2;
    const int gpu_bar_y = cpu_bar_y + BAR_HEIGHT + BAR_GAP;
    
    cairo_select_font_face(cr, "DejaVuSans-Bold", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    
    if (mode == DISPLAY_MODE_DEF) {
        // Spezielle Labels für def-Modus (weiß, größer, seitlich)
        cairo_set_font_size(cr, 26);
        cairo_set_source_rgb(cr, 1, 1, 1);
        
        cairo_text_extents_t text_ext;
        
        cairo_text_extents(cr, "CPU", &text_ext);
        cairo_move_to(cr, 1 + (50 - text_ext.width) / 2, 70);
        cairo_show_text(cr, "CPU");
        
        cairo_text_extents(cr, "GPU", &text_ext);
        cairo_move_to(cr, 1 + (50 - text_ext.width) / 2, DISPLAY_HEIGHT - 50);
        cairo_show_text(cr, "GPU");
    } else {
        // Labels für Modi 1, 2, 3 (schwarz, aligned mit Balken)
        cairo_set_font_size(cr, FONT_SIZE_LABELS);
        cairo_set_source_rgb(cr, 0, 0, 0);
        
        cairo_text_extents_t text_ext;
        
        cairo_text_extents(cr, "CPU", &text_ext);
        cairo_move_to(cr, 15 + (40 - text_ext.width) / 2, 
                      cpu_bar_y + (BAR_HEIGHT + text_ext.height) / 2);
        cairo_show_text(cr, "CPU");
        
        cairo_text_extents(cr, "GPU", &text_ext);
        cairo_move_to(cr, 15 + (40 - text_ext.width) / 2, 
                      gpu_bar_y + (BAR_HEIGHT + text_ext.height) / 2);
        cairo_show_text(cr, "GPU");
    }
}

/**
 * Zeichnet Temperatur-Displays (große Zahlen)
 */
static void draw_temperature_displays(cairo_t *cr, const sensor_data_t *data, display_mode_t mode __attribute__((unused))) {
    const int cpu_box_x = 2 + BOX_WIDTH + BOX_GAP;
    const int cpu_box_y = 2;
    const int coolant_box_x = 2 + 2 * (BOX_WIDTH + BOX_GAP);
    const int coolant_box_y = 2;
    const int gpu_box_x = 2 + BOX_WIDTH + BOX_GAP;
    const int gpu_box_y = DISPLAY_HEIGHT - BOX_HEIGHT - 2;
    
    cairo_select_font_face(cr, "DejaVuSans-Bold", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgb(cr, 1, 1, 1);
    
    char temp_str[8];
    char cool_str[16];
    cairo_text_extents_t ext, deg_ext, cool_ext, deg_ext_cool, deg_ext_gpu;

    // CPU-Temperaturanzeige
    snprintf(temp_str, sizeof(temp_str), "%d", (int)data->cpu_temp);
    cairo_set_font_size(cr, FONT_SIZE_LARGE);
    cairo_text_extents(cr, temp_str, &ext);
    const double cpu_temp_x = cpu_box_x + (BOX_WIDTH - ext.width) / 2;
    const double cpu_temp_y = cpu_box_y + ext.height + 8;
    cairo_move_to(cr, cpu_temp_x, cpu_temp_y);
    cairo_show_text(cr, temp_str);
    
    // Grad-Symbol für CPU
    cairo_set_font_size(cr, FONT_SIZE_DEGREE);
    cairo_text_extents(cr, "°", &deg_ext);
    cairo_move_to(cr, cpu_temp_x + ext.width + 8, cpu_temp_y - deg_ext.height + 4);
    cairo_show_text(cr, "°");

    // Coolant-Temperaturanzeige (nur wenn CPU <= 99°C)
    if (data->cpu_temp <= 99.0f && data->coolant_temp > 0.0f) {
        snprintf(cool_str, sizeof(cool_str), "%.1f", data->coolant_temp);
        cairo_set_font_size(cr, FONT_SIZE_COOLANT);
        cairo_text_extents(cr, cool_str, &cool_ext);
        const double cool_x = coolant_box_x + (BOX_WIDTH - cool_ext.width) / 2;
        const double cool_y = coolant_box_y + BOX_HEIGHT + 8;
        cairo_move_to(cr, cool_x + 4, cool_y);
        cairo_show_text(cr, cool_str);
        
        // Grad-Symbol für Coolant
        cairo_set_font_size(cr, FONT_SIZE_DEGREE * 0.30);
        cairo_text_extents(cr, "°", &deg_ext_cool);
        cairo_move_to(cr, cool_x + cool_ext.width + 8, cool_y - deg_ext_cool.height - 2);
        cairo_show_text(cr, "°");
    }

    // GPU-Temperaturanzeige
    snprintf(temp_str, sizeof(temp_str), "%d", (int)data->gpu_temp);
    cairo_set_font_size(cr, FONT_SIZE_LARGE);
    cairo_text_extents(cr, temp_str, &ext);
    const double gpu_temp_x = gpu_box_x + (BOX_WIDTH - ext.width) / 2;
    const double gpu_temp_y = gpu_box_y + BOX_HEIGHT - 8;
    cairo_move_to(cr, gpu_temp_x, gpu_temp_y);
    cairo_show_text(cr, temp_str);
    
    // Grad-Symbol für GPU
    cairo_set_font_size(cr, FONT_SIZE_DEGREE);
    cairo_text_extents(cr, "°", &deg_ext_gpu);
    cairo_move_to(cr, gpu_temp_x + ext.width + 8, gpu_temp_y - deg_ext_gpu.height + 4);
    cairo_show_text(cr, "°");
}

/**
 * Zeichnet vertikale Balken für Modus 1
 */
static void draw_mode_1_vertical_bars(cairo_t *cr, const sensor_data_t *data) {
    // CPU-Auslastungsbalken (blau)
    const int cpu_fill = (int)(VERTICAL_BAR_HEIGHT * data->cpu_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_CPU_USAGE_R, COLOR_CPU_USAGE_G, COLOR_CPU_USAGE_B);
    cairo_rectangle(cr, VERTICAL_BAR_BASE_X, VERTICAL_BAR_BASE_Y + VERTICAL_BAR_HEIGHT - cpu_fill, 
                   VERTICAL_BAR_WIDTH, cpu_fill);
    cairo_fill(cr);
    
    // RAM-Auslastungsbalken (lila)
    const int ram_fill = (int)(VERTICAL_BAR_HEIGHT * data->ram_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_RAM_USAGE_R, COLOR_RAM_USAGE_G, COLOR_RAM_USAGE_B);
    cairo_rectangle(cr, VERTICAL_BAR_BASE_X + VERTICAL_BAR_WIDTH + VERTICAL_BAR_SPACING, 
                   VERTICAL_BAR_BASE_Y + VERTICAL_BAR_HEIGHT - ram_fill, VERTICAL_BAR_WIDTH, ram_fill);
    cairo_fill(cr);

    // GPU-Auslastungsbalken (grün)
    const int gpu_y_base = DISPLAY_HEIGHT - VERTICAL_BAR_HEIGHT - 10;
    const int gpu_fill = (int)(VERTICAL_BAR_HEIGHT * data->gpu_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_GPU_USAGE_R, COLOR_GPU_USAGE_G, COLOR_GPU_USAGE_B);
    cairo_rectangle(cr, VERTICAL_BAR_BASE_X, gpu_y_base + VERTICAL_BAR_HEIGHT - gpu_fill, 
                   VERTICAL_BAR_WIDTH, gpu_fill);
    cairo_fill(cr);
    
    // GPU-RAM-Auslastungsbalken (lila)
    const int gpu_ram_fill = (int)(VERTICAL_BAR_HEIGHT * data->gpu_ram_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_GPU_RAM_USAGE_R, COLOR_GPU_RAM_USAGE_G, COLOR_GPU_RAM_USAGE_B);
    cairo_rectangle(cr, VERTICAL_BAR_BASE_X + VERTICAL_BAR_WIDTH + VERTICAL_BAR_SPACING, 
                   gpu_y_base + VERTICAL_BAR_HEIGHT - gpu_ram_fill, VERTICAL_BAR_WIDTH, gpu_ram_fill);
    cairo_fill(cr);
}

/**
 * Zeichnet Kreisdiagramme für Modus 2
 */
static void draw_mode_2_circles(cairo_t *cr, const sensor_data_t *data) {
    const double cy_bot = DISPLAY_HEIGHT - CIRCLE_CENTER_Y_TOP;
    
    // Oben: CPU-Auslastung (blau) und RAM-Auslastung (lila)
    // Hintergrundkreise (grau)
    cairo_set_line_width(cr, CIRCLE_THICKNESS);
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_arc(cr, CIRCLE_CENTER_X, CIRCLE_CENTER_Y_TOP, CIRCLE_RADIUS, 0, 2 * M_PI);
    cairo_stroke(cr);
    cairo_arc(cr, CIRCLE_CENTER_X, CIRCLE_CENTER_Y_TOP, CIRCLE_INNER_RADIUS, 0, 2 * M_PI);
    cairo_stroke(cr);
    
    // CPU-Auslastung (äußerer Ring, blau)
    const double cpu_angle = 2 * M_PI * data->cpu_usage / 100.0;
    cairo_set_source_rgb(cr, COLOR_CPU_USAGE_R, COLOR_CPU_USAGE_G, COLOR_CPU_USAGE_B);
    cairo_arc(cr, CIRCLE_CENTER_X, CIRCLE_CENTER_Y_TOP, CIRCLE_RADIUS, -M_PI/2, -M_PI/2 + cpu_angle);
    cairo_stroke(cr);
    
    // RAM-Auslastung (innerer Ring, lila)
    const double ram_angle = 2 * M_PI * data->ram_usage / 100.0;
    cairo_set_source_rgb(cr, COLOR_RAM_USAGE_R, COLOR_RAM_USAGE_G, COLOR_RAM_USAGE_B);
    cairo_arc(cr, CIRCLE_CENTER_X, CIRCLE_CENTER_Y_TOP, CIRCLE_INNER_RADIUS, -M_PI/2, -M_PI/2 + ram_angle);
    cairo_stroke(cr);

    // Unten: GPU-Auslastung (grün) und GPU-RAM-Auslastung (lila)
    // Hintergrundkreise (grau)
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_arc(cr, CIRCLE_CENTER_X, cy_bot, CIRCLE_RADIUS, 0, 2 * M_PI);
    cairo_stroke(cr);
    cairo_arc(cr, CIRCLE_CENTER_X, cy_bot, CIRCLE_INNER_RADIUS, 0, 2 * M_PI);
    cairo_stroke(cr);
    
    // GPU-Auslastung (äußerer Ring, grün)
    const double gpu_angle = 2 * M_PI * data->gpu_usage / 100.0;
    cairo_set_source_rgb(cr, COLOR_GPU_USAGE_R, COLOR_GPU_USAGE_G, COLOR_GPU_USAGE_B);
    cairo_arc(cr, CIRCLE_CENTER_X, cy_bot, CIRCLE_RADIUS, -M_PI/2, -M_PI/2 + gpu_angle);
    cairo_stroke(cr);
    
    // GPU-RAM-Auslastung (innerer Ring, lila)
    const double gpu_ram_angle = 2 * M_PI * data->gpu_ram_usage / 100.0;
    cairo_set_source_rgb(cr, COLOR_GPU_RAM_USAGE_R, COLOR_GPU_RAM_USAGE_G, COLOR_GPU_RAM_USAGE_B);
    cairo_arc(cr, CIRCLE_CENTER_X, cy_bot, CIRCLE_INNER_RADIUS, -M_PI/2, -M_PI/2 + gpu_ram_angle);
    cairo_stroke(cr);
}

/**
 * Zeichnet horizontale Balken für Modus 3
 */
static void draw_mode_3_horizontal_bars(cairo_t *cr, const sensor_data_t *data) {
    const int bar_x = (DISPLAY_WIDTH - BAR_WIDTH) / 2;
    const int cpu_bar_y = (DISPLAY_HEIGHT - (2 * BAR_HEIGHT + BAR_GAP)) / 2;
    const int gpu_bar_y = cpu_bar_y + BAR_HEIGHT + BAR_GAP;
    const int offset_x = bar_x + 1;
    const int max_bar_width = BAR_WIDTH - 2;
    
    // CPU-Auslastung (blau)
    const int cpu_usage_width = (int)(max_bar_width * data->cpu_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_CPU_USAGE_R, COLOR_CPU_USAGE_G, COLOR_CPU_USAGE_B);
    cairo_rectangle(cr, offset_x, cpu_bar_y + 1, cpu_usage_width, SMALL_BAR_HEIGHT);
    cairo_fill(cr);
    
    // RAM-Auslastung (lila)
    const int ram_usage_width = (int)(max_bar_width * data->ram_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_RAM_USAGE_R, COLOR_RAM_USAGE_G, COLOR_RAM_USAGE_B);
    cairo_rectangle(cr, offset_x, cpu_bar_y + 1 + SMALL_BAR_SPACING, ram_usage_width, SMALL_BAR_HEIGHT);
    cairo_fill(cr);
    
    // GPU-Auslastung (gelb)
    const int gpu_usage_width = (int)(max_bar_width * data->gpu_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_GPU_USAGE_YELLOW_R, COLOR_GPU_USAGE_YELLOW_G, COLOR_GPU_USAGE_YELLOW_B);
    cairo_rectangle(cr, offset_x, gpu_bar_y + 1, gpu_usage_width, SMALL_BAR_HEIGHT);
    cairo_fill(cr);
    
    // GPU-RAM-Auslastung (lila)
    const int gpu_ram_usage_width = (int)(max_bar_width * data->gpu_ram_usage / 100.0f);
    cairo_set_source_rgb(cr, COLOR_GPU_RAM_USAGE_R, COLOR_GPU_RAM_USAGE_G, COLOR_GPU_RAM_USAGE_B);
    cairo_rectangle(cr, offset_x, gpu_bar_y + 1 + SMALL_BAR_SPACING, gpu_ram_usage_width, SMALL_BAR_HEIGHT);
    cairo_fill(cr);
}

/**
 * Prüft ob Display-Update nötig ist (Änderungserkennung)
 */
static int should_update_display(const sensor_data_t *data) {
    static sensor_data_t last_data = {.cpu_temp = -1.0f, .gpu_temp = -1.0f, .coolant_temp = -1.0f, 
                                      .cpu_usage = -1.0f, .ram_usage = -1.0f, .gpu_usage = -1.0f, .gpu_ram_usage = -1.0f};
    static int first_run = 1;
    
    if (first_run) {
        first_run = 0;
        last_data = *data;
        return 1;
    }
    
    // Prüfe auf signifikante Änderungen
    if (fabsf(data->cpu_temp - last_data.cpu_temp) > CHANGE_TOLERANCE_TEMP ||
        fabsf(data->gpu_temp - last_data.gpu_temp) > CHANGE_TOLERANCE_TEMP ||
        fabsf(data->coolant_temp - last_data.coolant_temp) > CHANGE_TOLERANCE_TEMP ||
        fabsf(data->cpu_usage - last_data.cpu_usage) > CHANGE_TOLERANCE_USAGE ||
        fabsf(data->gpu_usage - last_data.gpu_usage) > CHANGE_TOLERANCE_USAGE ||
        fabsf(data->ram_usage - last_data.ram_usage) > CHANGE_TOLERANCE_USAGE ||
        fabsf(data->gpu_ram_usage - last_data.gpu_ram_usage) > CHANGE_TOLERANCE_USAGE) {
        
        last_data = *data;
        return 1;
    }
    
    return 0;
}

/**
 * Vereinfachte All-in-One Funktion - sammelt Sensordaten und rendert das Display
 * 
 * @param mode Display-Modus
 */
void draw_combined_image(display_mode_t mode) {
    sensor_data_t sensor_data = {0};
    
    // Temperaturen (in allen Modi)
    sensor_data.cpu_temp = read_cpu_temp();
    sensor_data.gpu_temp = read_gpu_temp();
    sensor_data.coolant_temp = read_coolant_temp();
    
    // Auslastungsdaten nur in Modi 1, 2, 3 (nicht in "def")
    if (mode != DISPLAY_MODE_DEF) {
        // CPU-Auslastung
        static cpu_stat_t last_cpu_stat = {0, 0};
        cpu_stat_t curr_cpu_stat;
        
        if (get_cpu_stat(&curr_cpu_stat)) {
            sensor_data.cpu_usage = calculate_cpu_usage(&last_cpu_stat, &curr_cpu_stat);
            last_cpu_stat = curr_cpu_stat;
        } else {
            sensor_data.cpu_usage = 0.0f;
        }
        
        // RAM-Auslastung
        sensor_data.ram_usage = get_ram_usage();
        
        // GPU-Auslastung
        float gpu_usage, gpu_mem_usage;
        if (get_gpu_usage_data(&gpu_usage, &gpu_mem_usage)) {
            sensor_data.gpu_usage = gpu_usage;
            sensor_data.gpu_ram_usage = gpu_mem_usage;
        } else {
            sensor_data.gpu_usage = 0.0f;
            sensor_data.gpu_ram_usage = 0.0f;
        }
    } else {
        // In "def"-Modus keine Auslastungsdaten
        sensor_data.cpu_usage = 0.0f;
        sensor_data.ram_usage = 0.0f;
        sensor_data.gpu_usage = 0.0f;
        sensor_data.gpu_ram_usage = 0.0f;
    }
    
    // Display rendern
    int render_result = render_display(&sensor_data, mode);
    if (render_result == 0) {
        // Stille Fortsetzung bei Render-Fehlern
        return;
    }
    
    // Bild an NZXT LCD senden (still, ohne Fehlermeldungen)
    upload_image_to_device(IMAGE_PATH, KRAKEN_UID);
}
