#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/**
 * LCD AIO CAM - Hauptprogramm
 * ===========================
 * 
 * Professioneller, modularer C-Daemon für NZXT Kraken LCD Temperature Monitor.
 * Zeigt CPU/GPU-Temperaturen und optional Auslastungsdaten auf dem LCD-Display an.
 * 
 * Kompiliert mit: make
 * Oder manuell: gcc -Wall -Wextra -O2 -std=c99 main.c cpu_monitor.c gpu_monitor.c 
 *               coolant_monitor.c display.c coolercontrol.c -o aiolcdcam -lcairo -lcurl -lm
 * 
 * Modi:
 *   def - Nur Temperaturen (CPU, GPU, Coolant)
 *   1   - Temperaturen + vertikale Auslastungsbalken 
 *   2   - Temperaturen + Kreisdiagramme für Auslastung
 *   3   - Temperaturen + horizontale Auslastungsbalken
 *
 * Single-Instance-Enforcement:
 *   - Nur eine Instanz von aiolcdcam kann gleichzeitig laufen
 *   - Bei manuellem Start: Fehler wenn systemd Service läuft
 *   - Bei systemd Start: Beendet vorherige manuelle Instanzen
 *   - PID-Datei koordiniert die Instanzverwaltung
 */

// Module einbinden
#include "../include/config.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/coolant_monitor.h"
#include "../include/display.h"
#include "../include/coolercontrol.h"

// Globale Variablen für Daemon-Management
static volatile sig_atomic_t running = 1; // Flag, ob der Daemon läuft
static volatile sig_atomic_t shutdown_sent = 0; // Flag, ob Shutdown-Image bereits gesendet wurde
static const char *pid_file = PID_FILE; // PID-Datei für den Daemon

/**
 * Signal-Handler für sauberes Beenden des Daemons mit Shutdown-Image
 */
static void cleanup_and_exit(int sig) {
    (void)sig; // Parameter wird nicht verwendet
    
    // Sende Shutdown-Image nur einmal
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = "/opt/aiolcdcam/image/face.png";
        printf("LCD AIO CAM: Sending shutdown image to Kraken LCD...\n");
        fflush(stdout);
        
        if (send_image_to_lcd(shutdown_image, KRAKEN_UID)) {
            printf("LCD AIO CAM: Shutdown image sent successfully\n");
            shutdown_sent = 1; // Flag setzen, damit es nur einmal gesendet wird
        } else {
            printf("LCD AIO CAM: Warning - Could not send shutdown image\n");
        }
        fflush(stdout);
    }
    
    unlink(pid_file); // Entferne die PID-Datei
    running = 0; // Setze Flag zum Beenden des Daemons
}

/**
 * Prüft auf bereits laufende Instanz und handhabe je nach Start-Typ
 * - Bei systemd Service: Beendet immer vorherige Instanz
 * - Bei manuellem Start: Verhindert Start wenn Service läuft, beendet andere manuelle Instanzen
 */
static int check_existing_instance_and_handle(const char *pid_file, int is_service_start) {
    FILE *f = fopen(pid_file, "r");
    if (!f) return 0; // Keine PID-Datei gefunden - alles ok
    
    pid_t old_pid;
    if (fscanf(f, "%d", &old_pid) != 1) {
        fclose(f);
        unlink(pid_file); // Ungültige PID-Datei entfernen
        return 0;
    }
    fclose(f);
    
    // Prüfe, ob die PID noch aktiv ist
    if (kill(old_pid, 0) != 0) {
        if (errno == ESRCH) {
            // Prozess existiert nicht mehr - alte PID-Datei entfernen
            unlink(pid_file);
            return 0;
        }
        // Bei EPERM existiert der Prozess wahrscheinlich (root process)
    }
    
    // Prüfe, ob es sich um einen systemd Service-Prozess handelt
    char cmdline_path[256];
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", old_pid);
    FILE *cmdline_f = fopen(cmdline_path, "r");
    int is_existing_service = 0;
    
    if (cmdline_f) {
        char cmdline[512] = {0};
        if (fread(cmdline, 1, sizeof(cmdline)-1, cmdline_f) > 0) {
            // Prüfe ob es von systemd gestartet wurde (enthält /opt/aiolcdcam/bin/aiolcdcam)
            if (strstr(cmdline, "/opt/aiolcdcam/bin/aiolcdcam") != NULL) {
                is_existing_service = 1;
            }
        }
        fclose(cmdline_f);
    }
    
    if (is_service_start) {
        // systemd Service startet: Beende immer vorherige Instanz
        printf("LCD AIO CAM: Service starting, terminating existing instance (PID %d)...\n", old_pid);
        kill(old_pid, SIGTERM);
        
        // Warte auf saubere Beendigung
        for (int i = 0; i < 50; i++) { // Max 5 Sekunden warten
            if (kill(old_pid, 0) != 0) break; // Prozess beendet
            usleep(100000); // 100ms warten
        }
        
        // Falls immer noch aktiv, erzwinge Beendigung
        if (kill(old_pid, 0) == 0) {
            printf("LCD AIO CAM: Force-killing stubborn instance...\n");
            kill(old_pid, SIGKILL);
            sleep(1);
        }
        
        printf("LCD AIO CAM: Previous instance terminated successfully\n");
        unlink(pid_file);
        return 0;
    } else {
        // Manueller Start: Prüfe ob Service läuft
        if (is_existing_service) {
            printf("LCD AIO CAM: Error - systemd service is already running (PID %d)\n", old_pid);
            printf("Stop the service first: sudo systemctl stop aiolcdcam.service\n");
            printf("Or check status: sudo systemctl status aiolcdcam.service\n");
            return -1; // Fehler: Service läuft bereits
        } else {
            // Andere manuelle Instanz läuft: Beende sie
            printf("LCD AIO CAM: Terminating existing manual instance (PID %d)...\n", old_pid);
            kill(old_pid, SIGTERM);
            
            // Warte auf saubere Beendigung
            for (int i = 0; i < 50; i++) { // Max 5 Sekunden warten
                if (kill(old_pid, 0) != 0) break; // Prozess beendet
                usleep(100000); // 100ms warten
            }
            
            // Falls immer noch aktiv, erzwinge Beendigung
            if (kill(old_pid, 0) == 0) {
                printf("LCD AIO CAM: Force-killing stubborn manual instance...\n");
                kill(old_pid, SIGKILL);
                sleep(1);
            }
            
            printf("LCD AIO CAM: Previous manual instance terminated successfully\n");
            unlink(pid_file);
            return 0;
        }
    }
}

/**
 * Schreibt die aktuelle PID in die PID-Datei
 */
static void write_pid_file(const char *pid_file) {
    FILE *f = fopen(pid_file, "w");
    if (f) {
        fprintf(f, "%d\n", getpid());
        fclose(f);
    }
}

/**
 * Main daemon loop
 */
static int run_daemon(display_mode_t mode) {
    printf("LCD AIO CAM daemon started (Mode: %d)\n", mode); // Show mode
    printf("Sensor data updated every %d.%d seconds\n", 
           DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC / 100000000);
    printf("Daemon now running silently in background...\n\n");
    fflush(stdout);
    
    while (running) { // Hauptschleife des Daemons
        draw_combined_image(mode); // Zeichne das kombinierte Bild basierend auf dem Modus
        struct timespec ts = {DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC}; // Wartezeit für die Aktualisierung
        nanosleep(&ts, NULL); // Warten für die angegebene Zeit
    }
    
    // Stilles Beenden ohne Ausgabe
    return 0;
}

/**
 * Zeigt die Hilfe an und erklärt die Verwendung des Programms
 */
static void show_help(const char *program_name) {
    printf("LCD AIO CAM - Complete NZXT Kraken LCD Temperature Monitor\n\n");
    printf("Usage: %s [MODE] or %s --mode [MODE]\n\n", program_name, program_name);
    printf("Modes:\n");
    printf("  def  - Temperatures only (default, resource-efficient)\n");
    printf("  1    - Temperatures + vertical load bars\n");
    printf("  2    - Temperatures + circular diagrams\n");
    printf("  3    - Temperatures + horizontal load bars\n\n");
    printf("Direct parameters:\n");
    printf("  %s def      # Show temperatures only\n", program_name);
    printf("  %s 1        # With vertical bars\n", program_name);
    printf("  %s 2        # With circular diagrams\n", program_name);
    printf("  %s 3        # With horizontal bars\n\n", program_name);
    printf("--mode parameters:\n");
    printf("  %s --mode def   # Show temperatures only\n", program_name);
    printf("  %s --mode 1     # With vertical bars\n", program_name);
    printf("  %s --mode 2     # With circular diagrams\n", program_name);
    printf("  %s --mode 3     # With horizontal bars\n\n", program_name);
    printf("The daemon runs in background and updates the LCD every %d.%d seconds.\n",
           DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC / 100000000);
    printf("To stop: sudo systemctl stop aiolcdcam\n");
}/**
 * Erkennt, ob wir von systemd gestartet wurden
 */
static int is_started_by_systemd(void) {
    // Einfache und zuverlässige Methode: Prüfe, ob unser Parent-Prozess PID 1 (init/systemd) ist
    return (getppid() == 1);
}

/**
 * Hauptfunktion des Programms
 */
int main(int argc, char *argv[]) {
    // Hilfe anzeigen
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        show_help(argv[0]);
        return 0;
    }
    
    // Display-Modus parsen - unterstützt beide Formate:
    // ./aiolcdcam def      oder    ./aiolcdcam --mode def
    // ./aiolcdcam 1        oder    ./aiolcdcam --mode 1
    // ./aiolcdcam 2        oder    ./aiolcdcam --mode 2
    // ./aiolcdcam 3        oder    ./aiolcdcam --mode 3
    // Standardmodus ist "def" (nur Temperaturen, ressourcenschonend)
    const char *mode_str = "def"; // Standard Modus
    
    if (argc > 1) { // Check if a parameter is provided
        // Check if the first parameter is --mode
        if (strcmp(argv[1], "--mode") == 0) { // --mode parameter
            // --mode parameter format: ./aiolcdcam --mode def
            // Check if a mode is specified
            if (argc > 2) {
                if (strcmp(argv[2], "def") == 0 || 
                    strcmp(argv[2], "1") == 0 || 
                    strcmp(argv[2], "2") == 0 || 
                    strcmp(argv[2], "3") == 0) {
                    mode_str = argv[2];
                } else {
                    fprintf(stderr, "Warning: Invalid mode '%s'! Fallback to 'def' mode.\n", argv[2]);
                    fprintf(stderr, "Valid modes: 'def', '1', '2', '3'\n");
                    fflush(stderr);
                    mode_str = "def";  // Fallback
                }
            } else {
                fprintf(stderr, "Warning: --mode requires a parameter! Fallback to 'def' mode.\n");
                fflush(stderr);
                mode_str = "def";  // Fallback
            }
        } else {
            // Direct parameter format: ./aiolcdcam def
            // Check if the parameter is valid
            if (strcmp(argv[1], "def") == 0 || 
                strcmp(argv[1], "1") == 0 || 
                strcmp(argv[1], "2") == 0 || 
                strcmp(argv[1], "3") == 0) {
                mode_str = argv[1];
            } else {
                fprintf(stderr, "Warning: Invalid mode '%s'! Fallback to 'def' mode.\n", argv[1]);
                fprintf(stderr, "Valid modes: 'def', '1', '2', '3'\n");
                fflush(stderr);
                mode_str = "def"; // Fallback
            }
        }
    }
    // Parse mode
    display_mode_t mode = parse_display_mode(mode_str); // Convert mode to enum
    
    // Show selected mode for systemd logs
    if (strcmp(mode_str, "def") != 0) { // If not default mode
        printf("Selected mode: %s (temperatures + load displays)\n", mode_str);
    } else { // Default mode
        printf("Selected mode: %s (temperatures only, resource-efficient)\n", mode_str);
    }
    fflush(stdout);
    
    // Prüfe, ob wir von systemd gestartet wurden
    int is_service_start = is_started_by_systemd();
    
    // Single-Instance-Enforcement: Prüfe und handhabe bereits laufende Instanzen
    if (check_existing_instance_and_handle(pid_file, is_service_start) < 0) {
        // Fehler: Service läuft bereits und wir sind manueller Start
        return 1;
    }
    
    // Schreibe neue PID-Datei
    write_pid_file(pid_file);
    
    // Register signal handlers
    signal(SIGTERM, cleanup_and_exit); // For systemd stop
    signal(SIGINT, cleanup_and_exit); // For Ctrl+C
    
    // Create image directory
    mkdir(IMAGE_DIR, 0755); // Create directory for images if not present
    
    // Initialize modules
    printf("Initializing modules...\n");
    fflush(stdout);
    
    // Initialize CPU sensors
    init_cpu_sensor_path(); // Set path to CPU sensors
    printf("✓ CPU monitor initialized\n");
    fflush(stdout);
    
    // Initialize GPU monitor (if GPU available)
    if (init_gpu_monitor()) { // Check return value
        printf("✓ GPU monitor initialized\n");
    } else {
        printf("⚠ GPU monitor not available (no NVIDIA GPU?)\n");
    }
    fflush(stdout);
    
    // Initialize coolant sensors
    init_coolant_sensor_path(); // Set path to coolant sensors
    printf("✓ Coolant monitor initialized\n");
    fflush(stdout);
    
    // Initialize CoolerControl session
    if (init_coolercontrol_session()) { // Check return value
        printf("✓ CoolerControl session initialized\n");
        
        // Get and display full Kraken device name
        char device_name[128] = {0};
        if (get_kraken_device_name(device_name, sizeof(device_name))) {
            printf("CoolerControl: Connected to %s\n", device_name);
        } else {
            printf("CoolerControl: Connected to Kraken LCD\n");
        }
        fflush(stdout);
    } else {
        fprintf(stderr, "Error: CoolerControl session could not be initialized\n");
        fprintf(stderr, "Please check:\n");
        fprintf(stderr, "  - Is coolercontrold running? (systemctl status coolercontrold)\n");
        fprintf(stderr, "  - Is the daemon running on localhost:11987?\n");
        fprintf(stderr, "  - Is the password correct? (see config.h)\n");
        fflush(stderr);
        return 1;
    }
    
    printf("All modules successfully initialized!\n\n");
    fflush(stdout);
    
    // Daemon starten
    int result = run_daemon(mode);
    
    // Cleanup - sende Shutdown-Image falls noch nicht gesendet (nur bei normaler Beendigung)
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = "/opt/aiolcdcam/image/face.png";
        printf("LCD AIO CAM: Sending final shutdown image...\n");
        fflush(stdout);
        
        if (send_image_to_lcd(shutdown_image, KRAKEN_UID)) {
            printf("LCD AIO CAM: Final shutdown image sent successfully\n");
        }
        fflush(stdout);
    }
    
    cleanup_coolercontrol_session(); // CoolerControl-Session beenden
    cleanup_and_exit(0); // PID-Datei entfernen und Daemon beenden
    
    return result;
}