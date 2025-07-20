/**
 * @file main.c
 * @brief Main entry point for CoolerDash daemon.
 *
 * Coding Standards (C99, ISO/IEC 9899:1999):
 * - All code comments in English.
 * - Doxygen-style comments for all functions (description, @brief, @param, @return, examples).
 * - Opening braces on the same line (K&R style).
 * - Always check return values of malloc(), calloc(), realloc().
 * - Free all dynamically allocated memory and set pointers to NULL after freeing.
 * - Use include guards in all headers.
 * - Include only necessary headers, system headers before local headers.
 * - Function names are verbs, use snake_case for functions/variables, UPPER_CASE for macros, PascalCase for typedef structs.
 * - Use descriptive names, avoid abbreviations.
 * - Document complex algorithms and data structures.
 */
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
 * Modular C daemon for LCD Temperature Monitor.
 * Displays CPU and GPU temperatures on an LCD display (two-box layout).
 *
 * Compile with: make
 * Or manually: gcc -Wall -Wextra -O2 -std=c99 main.c cpu_monitor.c gpu_monitor.c \
 *               display.c coolercontrol.c -o coolerdash -lcairo -lcurl -lm
 *
 * Features:
 *   - Displays CPU and GPU temperatures (no coolant, no load bars)
 *   - Resource-efficient, optimized for continuous background operation
 *   - Two-box layout: CPU (top), GPU (bottom)
 *   - Configurable colors and update interval (see config.h)
 *   - Single-Instance Enforcement: Only one coolerdash instance can run simultaneously
 *   - systemd start: Terminates previous manual instances
 *   - Manual start: Error if systemd service is running
 *   - PID file coordinates instance management
 *
 * Future:
 *   - Support for multiple display modes and GUI planned
 *
 * Usage:
 *   - Start via systemd service (recommended)
 *   - Manual start possible, but not parallel zum Service
 *   - See README.md for configuration and details
 */

// Include modules
#include "../include/config.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/display.h"
#include "../include/coolercontrol.h"

// Global variables for daemon management
static volatile sig_atomic_t running = 1; // flag whether daemon is running
static volatile sig_atomic_t shutdown_sent = 0; // flag whether shutdown image was already sent
static const char *pid_file = PID_FILE; // PID file for daemon

/**
 * @brief Signal handler for clean daemon termination with shutdown image.
 *
 * Sends a shutdown image to the LCD (if not already sent), removes the PID file, and sets the running flag to 0 for clean termination.
 *
 * @param sig Signal number (unused)
 * @return void
 *
 * Example:
 * @code
 * signal(SIGTERM, cleanup_and_exit);
 * @endcode
 */
static void cleanup_and_exit(int sig) {
    (void)sig; // parameter is not used
    // Send shutdown image only once
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = SHUTDOWN_IMAGE_PATH;
        const char* device_uid = get_cached_device_uid();
        printf("CoolerDash: Sending shutdown image to LCD...\n");
        fflush(stdout);
        if (device_uid[0]) {
            send_image_to_lcd(shutdown_image, device_uid);
            send_image_to_lcd(shutdown_image, device_uid);
            printf("CoolerDash: Shutdown image sent successfully\n");
            shutdown_sent = 1; // set flag so it's only sent once
        } else {
            printf("CoolerDash: Warning - Could not send shutdown image (device UID not detected)\n");
        }
        fflush(stdout);
    }
    unlink(pid_file); // remove PID file
    running = 0; // set flag to terminate daemon
}

/**
 * @brief Check for existing instance and handle based on start type.
 *
 * Checks for an existing running instance using the PID file. If started by systemd, always terminates the previous instance. If started manually, prevents start if a service is running, otherwise terminates other manual instances.
 *
 * @param pid_file Path to the PID file
 * @param is_service_start 1 if started by systemd, 0 if manual
 * @return 0 if no error, -1 if service already running (manual start)
 *
 * Example:
 * @code
 * if (check_existing_instance_and_handle(pid_file, is_service_start) < 0) {
 *     // Error: Service already running
 * }
 * @endcode
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
 * @brief Write current PID to PID file.
 *
 * Writes the current process ID to the specified PID file for single-instance enforcement.
 *
 * @param pid_file Path to the PID file
 * @return void
 *
 * Example:
 * @code
 * write_pid_file("/var/run/coolerdash.pid");
 * @endcode
 */
static void write_pid_file(const char *pid_file) {
    FILE *f = fopen(pid_file, "w");
    if (f) {
        fprintf(f, "%d\n", getpid());
        fclose(f);
    }
}

/**
 * @brief Main daemon loop.
 *
 * Runs the main loop of the daemon, periodically updating the display with sensor data until termination is requested.
 *
 * @return 0 on normal termination
 *
 * Example:
 * @code
 * int result = run_daemon();
 * @endcode
 */
static int run_daemon(void) {
    printf("CoolerDash daemon started (Temperatures only, resource-efficient)\n");
    printf("Sensor data updated every %d.%d seconds\n", 
           DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC / 100000000);
    printf("Daemon now running silently in background...\n\n");
    fflush(stdout);
    
    while (running) { // Main daemon loop
        draw_combined_image(); // Draw combined image (default mode only)
        struct timespec ts = {DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC}; // Wait time for update
        nanosleep(&ts, NULL); // Wait for specified time
    }
    
    // Silent termination without output
    return 0;
}

/**
 * @brief Show help and explain program usage.
 *
 * Prints usage information and help text to stdout.
 *
 * @param program_name Name of the executable
 * @return void
 *
 * Example:
 * @code
 * show_help(argv[0]);
 * @endcode
 */
static void show_help(const char *program_name) {
    printf("CoolerDash - Complete LCD Temperature Monitor\n\n");
    printf("Usage: %s\n\n", program_name);
    printf("This version only supports the default mode (temperatures only, resource-efficient).\n");
    printf("The daemon runs in background and updates the LCD every %d.%d seconds.\n",
           DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC / 100000000);
    printf("To stop: sudo systemctl stop coolerdash\n");
}

/**
 * @brief Detect if we were started by systemd.
 *
 * Checks if the parent process is PID 1 (systemd/init).
 *
 * @return 1 if started by systemd, 0 otherwise
 *
 * Example:
 * @code
 * int is_service = is_started_by_systemd();
 * @endcode
 */
static int is_started_by_systemd(void) {
    // Simple and reliable method: Check if our parent process is PID 1 (init/systemd)
    return (getppid() == 1);
}

/**
 * @brief Main function for CoolerDash daemon.
 *
 * Initializes modules, enforces single-instance, manages daemon lifecycle, and handles clean shutdown.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 on error
 *
 * Example:
 * @code
 * int main(int argc, char *argv[]) {
 *     return main(argc, argv);
 * }
 * @endcode
 */
int main(int argc, char *argv[]) {
    // Show help
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        show_help(argv[0]);
        return 0;
    }
    
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
    
    // Initialize CoolerControl session
    if (init_coolercontrol_session()) { // Check return value
        printf("✓ CoolerControl session initialized\n");
        
        // Get and display LCD device UID only if detected and changed
        static char last_device_uid[128] = {0};
        char device_uid[128] = {0};
        get_device_uid(device_uid, sizeof(device_uid));
        if (strcmp(last_device_uid, device_uid) != 0) {
            printf("CoolerControl: Detected LCD device UID: %.20s...\n", device_uid);
            strncpy(last_device_uid, device_uid, sizeof(last_device_uid));
        }
        
        // Get and display full LCD device name only if detected
        char device_name[128] = {0};
        if (get_device_name(device_name, sizeof(device_name))) {
            printf("CoolerControl: Connected to %s\n", device_name);
        } else {
            printf("CoolerControl: Connected to Unknow LCD device\n");
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
    if (!init_cached_device_uid()) {
        fprintf(stderr, "CoolerDash: Failed to detect LCD device UID\n");
        return 1;
    }
    printf("All modules successfully initialized!\n\n");
    fflush(stdout);
    
    // Start daemon
    int result = run_daemon();
    
    // Cleanup - send shutdown image if not sent yet (only on normal termination)
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = SHUTDOWN_IMAGE_PATH;
        const char* device_uid = get_cached_device_uid();
        printf("CoolerDash: Sending final shutdown image...\n");
        fflush(stdout);
        if (device_uid[0]) {
            send_image_to_lcd(shutdown_image, device_uid);
            send_image_to_lcd(shutdown_image, device_uid);
            printf("CoolerDash: Final shutdown image sent successfully\n");
        } else {
            printf("CoolerDash: Warning - Could not send final shutdown image (device UID not detected)\n");
        }
        fflush(stdout);
    }
    
    cleanup_coolercontrol_session(); // Terminate CoolerControl session
    cleanup_and_exit(0); // Remove PID file and terminate daemon
    
    return result;
}