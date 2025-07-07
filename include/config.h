#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// LCD AIO CAM CONFIGURATION
// =============================================================================

// Display Settings
#define DISPLAY_WIDTH 240 // 240 Pixel Breite
#define DISPLAY_HEIGHT 240 // 240 Pixel Höhe
#define DISPLAY_REFRESH_INTERVAL_SEC 2 // 2 Sekunden
#define DISPLAY_REFRESH_INTERVAL_NSEC 500000000  // 0.5 Sekunden

// Temperature Thresholds (für Farbverlauf)
#define TEMP_THRESHOLD_GREEN 55.0f // 55 Grad Celsius
#define TEMP_THRESHOLD_ORANGE 65.0f // 65 Grad Celsius
#define TEMP_THRESHOLD_RED 75.0f // 75 Grad Celsius

// Layout Constants
#define BOX_WIDTH 70 // 240 / 3.42857142857
#define BOX_HEIGHT 60 // 240 / 4
#define BOX_GAP 10 // Abstand zwischen den Boxen
#define BAR_WIDTH 220 // 240 - (BOX_WIDTH * 3) - (BOX_GAP * 2) - 10 (für den Rand)
#define BAR_HEIGHT 30 // Höhe der Balken
#define BAR_GAP 6 // Abstand zwischen den Balken

// Font Sizes
#define FONT_SIZE_LARGE 90.0  // BOX_HEIGHT * 1.50
#define FONT_SIZE_DEGREE 67.5 // FONT_SIZE_LARGE * 0.75
#define FONT_SIZE_COOLANT 24.0 // FONT_SIZE_LARGE * 0.30
#define FONT_SIZE_LABELS 22.0 // FONT_SIZE_LARGE * 0.25

// Cache Settings
#define GPU_CACHE_INTERVAL 2 // Sekunden
#define CHANGE_TOLERANCE_TEMP 0.1f // Temperaturänderungstoleranz in Grad Celsius
#define CHANGE_TOLERANCE_USAGE 0.5f // Nutzungsänderungstoleranz in Prozent

// Paths and Files
#define HWMON_PATH "/sys/class/hwmon" // Pfad zu den Hardware-Monitoren
#define IMAGE_DIR "/opt/aiolcdcam/image" // Verzeichnis für Bilder
#define IMAGE_PATH "/opt/aiolcdcam/image/cpu_gpu_temp.png" // Pfad zum Bild für CPU/GPU-Temperaturanzeige
#define PID_FILE "/var/run/aiolcdcam.pid" // PID-Datei für den Daemon

// CoolerControl Settings
// Stellen Sie sicher, dass CoolerControl läuft: sudo systemctl start coolercontrold
// Testen Sie die API: curl http://localhost:11987/devices
#define DAEMON_ADDRESS "http://localhost:11987" // Adresse des CoolerControl Daemon
#define DAEMON_PASSWORD "coolAdmin" // Passwort für CoolerControl Daemon

// NZXT Kraken UID (WICHTIG: Muss für Ihr Gerät angepasst werden!)
// 
// ⚠️  DIESE UUID MUSS FÜR IHR GERÄT GEÄNDERT WERDEN! ⚠️
// Anleitung siehe README.md Abschnitt "Device UUID Configuration"
//
// Kurz: curl http://localhost:11987/devices | jq → UUID kopieren → hier einfügen
// Getestet mit: NZXT Kraken 2023 (Z-Series)
//
#define KRAKEN_UID "8d4becb03bca2a8e8d4213ac376a1094f39d2786f688549ad3b6a591c3affdf9"

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
#define VERTICAL_BAR_BASE_X 10 // Basisposition der vertikalen Balken
#define VERTICAL_BAR_BASE_Y 10 // Basisposition der vertikalen Balken
#define VERTICAL_BAR_WIDTH 6 // Breite der vertikalen Balken
#define VERTICAL_BAR_HEIGHT (BOX_HEIGHT - 1) // Höhe der vertikalen Balken
#define VERTICAL_BAR_SPACING 6 // Abstand zwischen den vertikalen Balken

// Circle Settings (Mode 2)
#define CIRCLE_CENTER_X 26.0 // X-Position des Kreismittelpunkts
#define CIRCLE_CENTER_Y_TOP 26.0 // Y-Position des oberen Kreismittelpunkts
#define CIRCLE_RADIUS 22.0 // Radius des Kreises
#define CIRCLE_THICKNESS 6.0 // Dicke des Kreises
#define CIRCLE_INNER_RADIUS (CIRCLE_RADIUS - CIRCLE_THICKNESS - 2.0) // Innerer Radius des Kreises

// Small Bar Settings (Mode 3)
#define SMALL_BAR_HEIGHT 5 // Höhe der kleinen Balken
#define SMALL_BAR_SPACING 6 // Abstand zwischen den kleinen Balken

#endif // CONFIG_H
