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
 * CoolerDash - Main Program
 * =========================
 * 
 * Professional, modular C daemon for AIO LCD Temperature Monitor.
 * Displays CPU/GPU temperatures and optional load data on LCD display.
 * 
 * Compile with: make
 * Or manually: gcc -Wall -Wextra -O2 -std=c99 main.c cpu_monitor.c gpu_monitor.c 
 *               coolant_monitor.c display.c coolercontrol.c -o coolerdash -lcairo -lcurl -lm
 * 
 * Modes:
 *   def - Temperatures only (CPU, GPU, Coolant)
 *   1   - Temperatures + vertical load bars 
 *   2   - Temperatures + circular diagrams for load
 *   3   - Temperatures + horizontal load bars
 *
 * Single-Instance Enforcement:
 *   - Only one coolerdash instance can run simultaneously
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
        const char* shutdown_image = "/opt/coolerdash/images/face.png";
        const char* device_uuid = get_cached_aio_uuid();
        
        printf("CoolerDash: Sending shutdown image to AIO LCD...\n");
        fflush(stdout);
        
        if (device_uuid && send_image_to_lcd(shutdown_image, device_uuid)) {
            printf("CoolerDash: Shutdown image sent successfully\n");
            shutdown_sent = 1; // set flag so it's only sent once
        } else {
            printf("CoolerDash: Warning - Could not send shutdown image%s\n", 
                   device_uuid ? "" : " (device UUID not detected)");
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
            // Check if started by systemd (contains /opt/coolerdash/bin/coolerdash)
            if (strstr(cmdline, "/opt/coolerdash/bin/coolerdash") != NULL) {
                is_existing_service = 1;
            }
        }
        fclose(cmdline_f);
    }
    
    if (is_service_start) {
        // systemd service starting: Always terminate previous instance
        printf("CoolerDash: Service starting, terminating existing instance (PID %d)...\n", old_pid);
        kill(old_pid, SIGTERM);
        
        // Wait for clean termination
        for (int i = 0; i < 50; i++) { // Wait max 5 seconds
            if (kill(old_pid, 0) != 0) break; // Process terminated
            usleep(100000); // wait 100ms
        }
        
        // If still active, force termination
        if (kill(old_pid, 0) == 0) {
            printf("CoolerDash: Force-killing stubborn instance...\n");
            kill(old_pid, SIGKILL);
            sleep(1);
        }
        
        printf("CoolerDash: Previous instance terminated successfully\n");
        unlink(pid_file);
        return 0;
    } else {
        // Manual start: Check if service is running
        if (is_existing_service) {
            printf("CoolerDash: Error - systemd service is already running (PID %d)\n", old_pid);
            printf("Stop the service first: sudo systemctl stop coolerdash.service\n");
            printf("Or check status: sudo systemctl status coolerdash.service\n");
            return -1; // Error: Service already running
        } else {
            // Other manual instance running: Terminate it
            printf("CoolerDash: Terminating existing manual instance (PID %d)...\n", old_pid);
            kill(old_pid, SIGTERM);
            
            // Wait for clean termination
            for (int i = 0; i < 50; i++) { // Wait max 5 seconds
                if (kill(old_pid, 0) != 0) break; // Process terminated
                usleep(100000); // wait 100ms
            }
            
            // If still active, force termination
            if (kill(old_pid, 0) == 0) {
                printf("CoolerDash: Force-killing stubborn manual instance...\n");
                kill(old_pid, SIGKILL);
                sleep(1);
            }
            
            printf("CoolerDash: Previous manual instance terminated successfully\n");
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
    printf("CoolerDash daemon started (Mode: %d)\n", mode); // Show mode
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
    printf("CoolerDash - Complete AIO LCD Temperature Monitor\n\n");
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
    printf("To stop: sudo systemctl stop coolerdash\n");
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
    // ./coolerdash def      or    ./coolerdash --mode def
    // ./coolerdash 1        or    ./coolerdash --mode 1
    // ./coolerdash 2        or    ./coolerdash --mode 2
    // ./coolerdash 3        or    ./coolerdash --mode 3
    // Default mode is "def" (temperatures only, resource-efficient)
    const char *mode_str = "def"; // default mode
    
    if (argc > 1) { // Check if a parameter is provided
        // Check if the first parameter is --mode
        if (strcmp(argv[1], "--mode") == 0) { // --mode parameter
            // --mode parameter format: ./coolerdash --mode def
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
            // Direct parameter format: ./coolerdash def
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
        
        // Get and display AIO device UUID
        char device_uuid[128] = {0};
        if (get_aio_device_uuid(device_uuid, sizeof(device_uuid))) {
            printf("CoolerControl: Detected AIO device UUID: %.20s...\n", device_uuid);
        } else {
            fprintf(stderr, "Error: Could not detect AIO device UUID\n");
            fprintf(stderr, "Please check:\n");
            fprintf(stderr, "  - Is your AIO device connected?\n");
            fprintf(stderr, "  - Does your device support LCD display?\n");
            fprintf(stderr, "  - Run 'curl http://localhost:11987/devices' to see available devices\n");
            fflush(stderr);
            return 1;
        }
        
        // Get and display full AIO device name
        char device_name[128] = {0};
        if (get_aio_device_name(device_name, sizeof(device_name))) {
            printf("CoolerControl: Connected to %s\n", device_name);
        } else {
            printf("CoolerControl: Connected to AIO LCD\n");
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
        const char* shutdown_image = "/opt/coolerdash/images/face.png";
        const char* device_uuid = get_cached_aio_uuid();
        
        printf("CoolerDash: Sending final shutdown image...\n");
        fflush(stdout);
        
        if (device_uuid && send_image_to_lcd(shutdown_image, device_uuid)) {
            printf("CoolerDash: Final shutdown image sent successfully\n");
        } else {
            printf("CoolerDash: Warning - Could not send final shutdown image%s\n",
                   device_uuid ? "" : " (device UUID not detected)");
        }
        fflush(stdout);
    }
    
    cleanup_coolercontrol_session(); // Terminate CoolerControl session
    cleanup_and_exit(0); // Remove PID file and terminate daemon
    
    return result;
}