#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// LCD AIO CAM CONFIGURATION
// =============================================================================

// Display Settings
#define DISPLAY_WIDTH 240 // 240 pixel width
#define DISPLAY_HEIGHT 240 // 240 pixel height
#define DISPLAY_REFRESH_INTERVAL_SEC 2 // 2 seconds
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
#define BOX_WIDTH 80 // 240 / 3
#define BOX_HEIGHT 80 // 240 / 3
#define BOX_GAP 0 // no gap for perfect 3x80 grid
#define BAR_WIDTH 220 // 240 - (BOX_WIDTH * 3) - (BOX_GAP * 2) - 10 (for border)
#define BAR_HEIGHT 20 // height of bars
#define BAR_GAP 0 // spacing between bars

// Font Sizes
#define FONT_SIZE_LARGE 100.0  // BOX_HEIGHT * 1.50
#define FONT_SIZE_DEGREE 80.0 // FONT_SIZE_LARGE * 0.75
#define FONT_SIZE_COOLANT 26.0 // FONT_SIZE_LARGE * 0.30
#define FONT_SIZE_LABELS 32.0 // FONT_SIZE_LARGE * 0.25

// LCD Display Settings
#define LCD_BRIGHTNESS 80 // Default LCD brightness (0-100)

// Cache Settings
#define GPU_CACHE_INTERVAL 2.5f // seconds (supports decimal values with millisecond precision)
#define CHANGE_TOLERANCE_TEMP 0.1f // temperature change tolerance in degrees Celsius
#define CHANGE_TOLERANCE_USAGE 0.5f // usage change tolerance in percent

// Paths and Files
#define HWMON_PATH "/sys/class/hwmon" // path to hardware monitors
#define IMAGE_DIR "/opt/coolerdash/images" // directory for images
#define IMAGE_PATH "/opt/coolerdash/images/coolerdash.png" // path to CPU/GPU temperature display image
#define PID_FILE "/var/run/coolerdash.pid" // PID file for daemon

// CoolerControl Settings
// Make sure CoolerControl is running: sudo systemctl start coolercontrold
// Test the API: curl http://localhost:11987/devices
#define DAEMON_ADDRESS "http://localhost:11987" // CoolerControl daemon address
#define DAEMON_PASSWORD "coolAdmin" // CoolerControl daemon passwor

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

// Background Colors
#define COLOR_BG_R 0.16
#define COLOR_BG_G 0.16
#define COLOR_BG_B 0.16

#define COLOR_BORDER_R 0.08
#define COLOR_BORDER_G 0.08
#define COLOR_BORDER_B 0.08

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

// Bar Settings (Mode 1)
#define VERTICAL_BAR_BASE_X 10 // base position of vertical bars
#define VERTICAL_BAR_BASE_Y 10 // base position of vertical bars
#define VERTICAL_BAR_WIDTH 6 // width of vertical bars
#define VERTICAL_BAR_HEIGHT (BOX_HEIGHT - 1) // height of vertical bars
#define VERTICAL_BAR_SPACING 6 // spacing between vertical bars

// Circle Settings (Mode 2)
#define CIRCLE_CENTER_X 26.0 // X position of circle center
#define CIRCLE_CENTER_Y_TOP 26.0 // Y position of upper circle center
#define CIRCLE_RADIUS 22.0 // circle radius
#define CIRCLE_THICKNESS 6.0 // circle thickness
#define CIRCLE_INNER_RADIUS (CIRCLE_RADIUS - CIRCLE_THICKNESS - 2.0) // inner circle radius

// Small Bar Settings (Mode 3)
#define SMALL_BAR_HEIGHT 5 // height of small bars
#define SMALL_BAR_SPACING 6 // spacing between small bars

#endif // CONFIG_H
