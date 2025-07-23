/**
 * @file config.h
 * @brief Central configuration header for CoolerDash.
 *
 * This header defines all static default values (macros) and the main Config struct.
 * All function comments use Doxygen style. See below for struct and function documentation.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <ini.h>

/* --- Static Defaults (Macros) --- */
// Display Settings
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240
#define DISPLAY_REFRESH_INTERVAL_SEC 3
#define DISPLAY_REFRESH_INTERVAL_NSEC 0

// Temperature Thresholds
#define TEMP_THRESHOLD_GREEN 55.0f
#define TEMP_THRESHOLD_ORANGE 65.0f
#define TEMP_THRESHOLD_RED 75.0f

// Layout Constants
#define BOX_WIDTH 240
#define BOX_HEIGHT 120
#define BOX_GAP 0
#define BAR_WIDTH 230
#define BAR_HEIGHT 22
#define BAR_GAP 10
#define BORDER_LINE_WIDTH 1.5

// Font Sizes
#define FONT_FACE "Roboto Black"
#define FONT_SIZE_LARGE 98.0
#define FONT_SIZE_LABELS 28.0

// LCD Display Settings
#define LCD_BRIGHTNESS 100
#define LCD_ORIENTATION "0"

// Cache Settings
#define GPU_CACHE_INTERVAL 3.0f
#define CHANGE_TOLERANCE_TEMP 1.0f
#define CHANGE_TOLERANCE_USAGE 1.0f

// Paths and Files
#define HWMON_PATH "/sys/class/hwmon"
#define IMAGE_DIR "/opt/coolerdash/images"
#define IMAGE_PATH "/dev/shm/coolerdash.png"
#define SHUTDOWN_IMAGE_PATH "/opt/coolerdash/images/shutdown.png"
#define PID_FILE "/var/run/coolerdash.pid"

// CoolerControl Settings
#define DAEMON_ADDRESS "http://localhost:11987"
#define DAEMON_PASSWORD "coolAdmin"

// Colors (RGB 0-255)
#define COLOR_GREEN_R 0
#define COLOR_GREEN_G 255
#define COLOR_GREEN_B 0

#define COLOR_ORANGE_R 255
#define COLOR_ORANGE_G 140
#define COLOR_ORANGE_B 0

#define COLOR_HOT_ORANGE_R 255
#define COLOR_HOT_ORANGE_G 70
#define COLOR_HOT_ORANGE_B 0

#define COLOR_RED_R 255
#define COLOR_RED_G 0
#define COLOR_RED_B 0

// Background Colors bar (RGB 0.0-1.0)
#define COLOR_BG_R 0.20
#define COLOR_BG_G 0.20
#define COLOR_BG_B 0.20

#define COLOR_BORDER_R 0.70
#define COLOR_BORDER_G 0.70
#define COLOR_BORDER_B 0.70

// Usage Bar Colors (RGB 0.0-1.0)
#define COLOR_CPU_USAGE_R 0.3
#define COLOR_CPU_USAGE_G 0.7
#define COLOR_CPU_USAGE_B 1.0

#define COLOR_RAM_USAGE_R 0.8
#define COLOR_RAM_USAGE_G 0.5
#define COLOR_RAM_USAGE_B 1.0

#define COLOR_GPU_USAGE_R 0.4
#define COLOR_GPU_USAGE_G 1.0
#define COLOR_GPU_USAGE_B 0.4

#define COLOR_GPU_RAM_USAGE_R 0.8
#define COLOR_GPU_RAM_USAGE_G 0.5
#define COLOR_GPU_RAM_USAGE_B 1.0

#define COLOR_GPU_USAGE_YELLOW_R 1.0
#define COLOR_GPU_USAGE_YELLOW_G 1.0
#define COLOR_GPU_USAGE_YELLOW_B 0.0

#define COLOR_TEMP_R 1.0
#define COLOR_TEMP_G 1.0
#define COLOR_TEMP_B 1.0

#define COLOR_LABEL_R 0.75
#define COLOR_LABEL_G 0.75
#define COLOR_LABEL_B 0.75

/* --- Dynamic Config Struct --- */
/**
 * @brief Structure for runtime configuration loaded from INI file.
 *
 * All fields are initialized with static defaults and can be overwritten by INI values.
 */
typedef struct Config {
    int display_width;
    int display_height;
    int display_refresh_interval_sec;
    int display_refresh_interval_nsec;
    int lcd_brightness;
    int lcd_orientation;
    int box_width;
    int box_height;
    int box_gap;
    int bar_width;
    int bar_height;
    int bar_gap;
    float border_line_width;
    char font_face[64];
    float font_size_large;
    float font_size_labels;
    float temp_threshold_green;
    float temp_threshold_orange;
    float temp_threshold_red;
    float gpu_cache_interval;
    float change_tolerance_temp;
    float change_tolerance_usage;
    char hwmon_path[128];
    char image_dir[128];
    char image_path[128];
    char shutdown_image[128];
    char pid_file[128];
    char daemon_address[128];
    char daemon_password[64];
    struct { int r, g, b; } color_green;
    struct { int r, g, b; } color_orange;
    struct { int r, g, b; } color_hot_orange;
    struct { int r, g, b; } color_red;
    struct { float r, g, b; } color_bg;
    struct { float r, g, b; } color_border;
    struct { float r, g, b; } color_cpu_usage;
    struct { float r, g, b; } color_ram_usage;
    struct { float r, g, b; } color_gpu_usage;
    struct { float r, g, b; } color_gpu_ram_usage;
    struct { float r, g, b; } color_gpu_usage_yellow;
    struct { float r, g, b; } color_temp;
    struct { float r, g, b; } color_label;
} Config;

/**
 * @brief Loads configuration from INI file.
 *
 * Loads all values from the given INI file and overwrites defaults in the Config struct.
 *
 * @param config Pointer to Config structure to fill.
 * @param path Path to INI file.
 * @return 0 on success, -1 on error.
 *
 * Example:
 * @code
 * Config cfg;
 * if (load_config_ini(&cfg, "/etc/coolerdash/config.ini") != 0) {
 *     // fallback to defaults
 * }
 * @endcode
 */
int load_config_ini(Config *config, const char *path);

#endif // CONFIG_H