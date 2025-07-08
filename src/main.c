#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
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
 * LCD AIO CAM - Main Program
 * ==========================
 * 
 * Professional, modular C daemon for NZXT Kraken LCD Temperature Monitor.
 * Displays CPU/GPU temperatures and optional load data on LCD display.
 * 
 * Compile with: make
 * Or manually: gcc -Wall -Wextra -O2 -std=c99 main.c cpu_monitor.c gpu_monitor.c 
 *               coolant_monitor.c display.c coolercontrol.c -o aiolcdcam -lcairo -lcurl -lm
 * 
 * Modes:
 *   def - Temperatures only (CPU, GPU, Coolant)
 *   1   - Temperatures + vertical load bars 
 *   2   - Temperatures + circular diagrams for load
 *   3   - Temperatures + horizontal load bars
 *
 * Single-Instance Enforcement:
 *   - Only one aiolcdcam instance can run simultaneously
 *   - Manual start: Error if systemd service is running
 *   - systemd start: Terminates previous manual instances
 *   - PID file coordinates instance management
 */

// Include modules
#include "../include/config.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/coolant_monitor.h"
#include "../include/display.h"
#include "../include/coolercontrol.h"

// Global variables for daemon management
static volatile sig_atomic_t running = 1; // flag whether daemon is running
static volatile sig_atomic_t shutdown_sent = 0; // flag whether shutdown image was already sent
static const char *pid_file = PID_FILE; // PID file for daemon

/**
 * Signal handler for clean daemon termination with shutdown image
 */
static void cleanup_and_exit(int sig) {
    (void)sig; // parameter is not used
    
    // Send shutdown image only once
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = "/opt/aiolcdcam/image/face.png";
        printf("LCD AIO CAM: Sending shutdown image to Kraken LCD...\n");
        fflush(stdout);
        
        if (send_image_to_lcd(shutdown_image, KRAKEN_UID)) {
            printf("LCD AIO CAM: Shutdown image sent successfully\n");
            shutdown_sent = 1; // set flag so it's only sent once
        } else {
            printf("LCD AIO CAM: Warning - Could not send shutdown image\n");
        }
        fflush(stdout);
    }
    
    unlink(pid_file); // remove PID file
    running = 0; // set flag to terminate daemon
}

/**
 * Check for existing instance and handle based on start type
 * - systemd service: Always terminates previous instance
 * - Manual start: Prevents start if service is running, terminates other manual instances
 */
static int check_existing_instance_and_handle(const char *pid_file, int is_service_start) {
    FILE *f = fopen(pid_file, "r");
    if (!f) return 0; // No PID file found - all ok
    
    pid_t old_pid;
    if (fscanf(f, "%d", &old_pid) != 1) {
        fclose(f);
        unlink(pid_file); // Remove invalid PID file
        return 0;
    }
    fclose(f);
    
    // Check if PID is still active
    if (kill(old_pid, 0) != 0) {
        if (errno == ESRCH) {
            // Process no longer exists - remove old PID file
            unlink(pid_file);
            return 0;
        }
        // With EPERM process probably exists (root process)
    }
    
    // Check if it's a systemd service process
    char cmdline_path[256];
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", old_pid);
    FILE *cmdline_f = fopen(cmdline_path, "r");
    int is_existing_service = 0;
    
    if (cmdline_f) {
        char cmdline[512] = {0};
        if (fread(cmdline, 1, sizeof(cmdline)-1, cmdline_f) > 0) {
            // Check if started by systemd (contains /opt/aiolcdcam/bin/aiolcdcam)
            if (strstr(cmdline, "/opt/aiolcdcam/bin/aiolcdcam") != NULL) {
                is_existing_service = 1;
            }
        }
        fclose(cmdline_f);
    }
    
    if (is_service_start) {
        // systemd service starting: Always terminate previous instance
        printf("LCD AIO CAM: Service starting, terminating existing instance (PID %d)...\n", old_pid);
        kill(old_pid, SIGTERM);
        
        // Wait for clean termination
        for (int i = 0; i < 50; i++) { // Wait max 5 seconds
            if (kill(old_pid, 0) != 0) break; // Process terminated
            usleep(100000); // wait 100ms
        }
        
        // If still active, force termination
        if (kill(old_pid, 0) == 0) {
            printf("LCD AIO CAM: Force-killing stubborn instance...\n");
            kill(old_pid, SIGKILL);
            sleep(1);
        }
        
        printf("LCD AIO CAM: Previous instance terminated successfully\n");
        unlink(pid_file);
        return 0;
    } else {
        // Manual start: Check if service is running
        if (is_existing_service) {
            printf("LCD AIO CAM: Error - systemd service is already running (PID %d)\n", old_pid);
            printf("Stop the service first: sudo systemctl stop aiolcdcam.service\n");
            printf("Or check status: sudo systemctl status aiolcdcam.service\n");
            return -1; // Error: Service already running
        } else {
            // Other manual instance running: Terminate it
            printf("LCD AIO CAM: Terminating existing manual instance (PID %d)...\n", old_pid);
            kill(old_pid, SIGTERM);
            
            // Wait for clean termination
            for (int i = 0; i < 50; i++) { // Wait max 5 seconds
                if (kill(old_pid, 0) != 0) break; // Process terminated
                usleep(100000); // wait 100ms
            }
            
            // If still active, force termination
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
 * Write current PID to PID file
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
    
    while (running) { // Main daemon loop
        draw_combined_image(mode); // Draw combined image based on mode
        struct timespec ts = {DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC}; // Wait time for update
        nanosleep(&ts, NULL); // Wait for specified time
    }
    
    // Silent termination without output
    return 0;
}

/**
 * Show help and explain program usage
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
 * Detect if we were started by systemd
 */
static int is_started_by_systemd(void) {
    // Simple and reliable method: Check if our parent process is PID 1 (init/systemd)
    return (getppid() == 1);
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    // Show help
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        show_help(argv[0]);
        return 0;
    }
    
    // Parse display mode - supports both formats:
    // ./aiolcdcam def      or    ./aiolcdcam --mode def
    // ./aiolcdcam 1        or    ./aiolcdcam --mode 1
    // ./aiolcdcam 2        or    ./aiolcdcam --mode 2
    // ./aiolcdcam 3        or    ./aiolcdcam --mode 3
    // Default mode is "def" (temperatures only, resource-efficient)
    const char *mode_str = "def"; // default mode
    
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
    
    // Check if we were started by systemd
    int is_service_start = is_started_by_systemd();
    
    // Single-Instance Enforcement: Check and handle existing instances
    if (check_existing_instance_and_handle(pid_file, is_service_start) < 0) {
        // Error: Service already running and we are manual start
        return 1;
    }
    
    // Write new PID file
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
    
    // Start daemon
    int result = run_daemon(mode);
    
    // Cleanup - send shutdown image if not sent yet (only on normal termination)
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = "/opt/aiolcdcam/image/face.png";
        printf("LCD AIO CAM: Sending final shutdown image...\n");
        fflush(stdout);
        
        if (send_image_to_lcd(shutdown_image, KRAKEN_UID)) {
            printf("LCD AIO CAM: Final shutdown image sent successfully\n");
        }
        fflush(stdout);
    }
    
    cleanup_coolercontrol_session(); // Terminate CoolerControl session
    cleanup_and_exit(0); // Remove PID file and terminate daemon
    
    return result;
}