/**
 * @file config.h
 * @brief Central configuration header for CoolerDash.
 *
 * Coding Standards (C99, ISO/IEC 9899:1999):
 * - Use english language for all code comments.
 * - Use Doxygen-style comments for functions. Include a description, @brief, @param, @return values and examples if useful.
 * - Opening braces on the same line for functions and control structures (K&R style).
 * - Add comments for non-obvious code sections.
 * - Always check return values of malloc(), calloc(), realloc().
 * - Free all dynamically allocated memory and set pointers to NULL after freeing.
 * - Use include guards: #ifndef HEADER_H / #define HEADER_H / #endif in all headers.
 * - Include only necessary headers. Separate system headers from local headers.
 * - Function names should be verbs: calculate_sum(), parse_input().
 * - Use snake_case for function names and variables: my_function(), user_count.
 * - Use UPPER_CASE for constants and macros: MAX_SIZE, PI.
 * - Use PascalCase for typedef structs: MyStruct.
 * - Use forward declarations when possible.
 * - Use descriptive names, avoid abbreviations.
 * - Document complex algorithms and data structures.
 */
#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// LCD CONFIGURATION
// =============================================================================

// Display Settings
#define DISPLAY_WIDTH 240 // 240 pixel width
#define DISPLAY_HEIGHT 240 // 240 pixel height
#define DISPLAY_REFRESH_INTERVAL_SEC 2 //  seconds
#define DISPLAY_REFRESH_INTERVAL_NSEC 500000000  // 0.5 seconds

// Temperature Thresholds (4-stage color gradient: green → orange → hot orange → red)
// Stage 1: Green (≤55°C) - Cool operating temperature
// Stage 2: Orange (≤65°C) - Warm operating temperature  
// Stage 3: Hot Orange (≤75°C) - Hot but acceptable temperature
// Stage 4: Red (>75°C) - Critical temperature warning
#define TEMP_THRESHOLD_GREEN 55.0f // 55°C - Green threshold
#define TEMP_THRESHOLD_ORANGE 65.0f // 65°C - Orange threshold  
#define TEMP_THRESHOLD_RED 75.0f // 75°C - Hot Orange threshold (above this = Red)

// Layout Constants
#define BOX_WIDTH 240 // full width for two boxes
#define BOX_HEIGHT 120 // half height for two boxes
#define BOX_GAP 0 // no gap for perfect 2x120 grid
#define BAR_WIDTH 230 // 240 - (BOX_WIDTH * 3) - (BOX_GAP * 2) - 10 (for border)
#define BAR_HEIGHT 22 // height of bars
#define BAR_GAP 10 // spacing between bars
#define BORDER_LINE_WIDTH 1.5 // Line width for box and bar borders (in pixels)

// Font Sizes
#define FONT_FACE "Roboto Black" // Font face for all display text
#define FONT_SIZE_LARGE 98.0  // BOX_HEIGHT * 1.50
#define FONT_SIZE_LABELS 28.0 // FONT_SIZE_LARGE * 0.25

// LCD Display Settings
#define LCD_BRIGHTNESS 100 // Default LCD brightness (0-100)
#define LCD_ORIENTATION "0" // LCD orientation for image upload ("0"=default, "90"=rotated, "180"=flipped, "270"=rotated flipped)

// Cache Settings
#define GPU_CACHE_INTERVAL 2.5f // seconds (supports decimal values with millisecond precision)
#define CHANGE_TOLERANCE_TEMP 1.0f // temperature change tolerance in degrees Celsius
#define CHANGE_TOLERANCE_USAGE 1.0f // usage change tolerance in percent

// Paths and Files
#define HWMON_PATH "/sys/class/hwmon" // path to hardware monitors
#define IMAGE_DIR "/opt/coolerdash/images" // directory for images
#define IMAGE_PATH "/dev/shm/coolerdash.png" // path to CPU/GPU temperature display image
#define SHUTDOWN_IMAGE_PATH "/opt/coolerdash/images/shutdown.png" // path to shutdown image
#define PID_FILE "/var/run/coolerdash.pid" // PID file for daemon

// CoolerControl Settings
// Make sure CoolerControl is running: sudo systemctl start coolercontrold
// Test the API: curl http://localhost:11987/devices | jq
#define DAEMON_ADDRESS "http://localhost:11987" // CoolerControl daemon address
#define DAEMON_PASSWORD "coolAdmin" // CoolerControl daemon password

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

// Background Colors bar
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

// Foreground/Text Color (for all temperature numbers)
#define COLOR_TEMP_R 1.0
#define COLOR_TEMP_G 1.0
#define COLOR_TEMP_B 1.0

// Foreground/Text Color (for all labels)
#define COLOR_LABEL_R 0.75
#define COLOR_LABEL_G 0.75
#define COLOR_LABEL_B 0.75

#endif // CONFIG_H
