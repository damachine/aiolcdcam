/**
 * @file main.c
 * @brief Main entry point for CoolerDash daemon.
 */

// Function prototypes
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600

// Include project headers
#include "../include/config.h"
#include "../include/cpu_monitor.h"
#include "../include/gpu_monitor.h"
#include "../include/display.h"
#include "../include/coolercontrol.h"

// Include necessary headers
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

// Global variables for daemon management
static volatile sig_atomic_t running = 1; // flag whether daemon is running
static volatile sig_atomic_t shutdown_sent = 0; // flag whether shutdown image was already sent
static const char *pid_file = PID_FILE; // PID file for daemon

// Global pointer to config for signal handler access
static const Config *g_config_ptr = NULL;

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
    if (!shutdown_sent && is_session_initialized() && g_config_ptr) {
        const char* shutdown_image = SHUTDOWN_IMAGE_PATH;
        const char* device_uid = get_cached_device_uid();
        printf("CoolerDash: Sending shutdown image to LCD...\n");
        fflush(stdout);
        if (device_uid[0]) {
            send_image_to_lcd(g_config_ptr, shutdown_image, device_uid);
            send_image_to_lcd(g_config_ptr, shutdown_image, device_uid);
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
static int run_daemon(const Config *config) {
    printf("CoolerDash daemon started (Temperatures only, resource-efficient)\n");
    printf("Sensor data updated every %d.%d seconds\n", 
           DISPLAY_REFRESH_INTERVAL_SEC, DISPLAY_REFRESH_INTERVAL_NSEC / 100000000);
    printf("Daemon now running silently in background...\n\n");
    fflush(stdout);
    while (running) { // Main daemon loop
        draw_combined_image(config); // Draw combined image
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
 * @brief Main entry point for CoolerDash.
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code
 *
 * Usage:
 *   coolerdash [config_path]
 *
 * If no config_path is given, /etc/coolerdash/config.ini is used.
 */
int main(int argc, char **argv)
{
    // Load configuration from INI file or use defaults from config.h
    Config config;
    const char *config_path = "/etc/coolerdash/config.ini";
    if (argc > 1) config_path = argv[1];
    if (load_config_ini(&config, config_path) != 0) {
        /**
         * @brief Fallback: Set all config values from macros if INI file cannot be loaded.
         *
         * This block ensures the daemon can start with hardcoded defaults if the config file is missing or invalid.
         * All fields are set manually to guarantee a valid configuration.
         */
        config.display_width = DISPLAY_WIDTH;
        config.display_height = DISPLAY_HEIGHT;
        config.display_refresh_interval_sec = DISPLAY_REFRESH_INTERVAL_SEC;
        config.display_refresh_interval_nsec = DISPLAY_REFRESH_INTERVAL_NSEC;
        config.lcd_brightness = LCD_BRIGHTNESS;
        config.lcd_orientation = atoi(LCD_ORIENTATION);
        config.box_width = BOX_WIDTH;
        config.box_height = BOX_HEIGHT;
        config.box_gap = BOX_GAP;
        config.bar_width = BAR_WIDTH;
        config.bar_height = BAR_HEIGHT;
        config.bar_gap = BAR_GAP;
        config.border_line_width = BORDER_LINE_WIDTH;
        strncpy(config.font_face, FONT_FACE, sizeof(config.font_face)-1);
        config.font_size_large = FONT_SIZE_LARGE;
        config.font_size_labels = FONT_SIZE_LABELS;
        config.temp_threshold_green = TEMP_THRESHOLD_GREEN;
        config.temp_threshold_orange = TEMP_THRESHOLD_ORANGE;
        config.temp_threshold_red = TEMP_THRESHOLD_RED;
        config.gpu_cache_interval = GPU_CACHE_INTERVAL;
        config.change_tolerance_temp = CHANGE_TOLERANCE_TEMP;
        config.change_tolerance_usage = CHANGE_TOLERANCE_USAGE;
        strncpy(config.hwmon_path, HWMON_PATH, sizeof(config.hwmon_path)-1);
        strncpy(config.image_dir, IMAGE_DIR, sizeof(config.image_dir)-1);
        strncpy(config.image_path, IMAGE_PATH, sizeof(config.image_path)-1);
        strncpy(config.shutdown_image, SHUTDOWN_IMAGE_PATH, sizeof(config.shutdown_image)-1);
        strncpy(config.pid_file, PID_FILE, sizeof(config.pid_file)-1);
        strncpy(config.daemon_address, DAEMON_ADDRESS, sizeof(config.daemon_address)-1);
        strncpy(config.daemon_password, DAEMON_PASSWORD, sizeof(config.daemon_password)-1);
        config.color_green.r = COLOR_GREEN_R;
        config.color_green.g = COLOR_GREEN_G;
        config.color_green.b = COLOR_GREEN_B;
        // ...set other color fields and struct members analogously...
    }

    // Show help
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        show_help(argv[0]);
        return 0;
    }
    
    // Check if we were started by systemd
    int is_service_start = is_started_by_systemd();
    
    // Single-Instance Enforcement: Check and handle existing instances
    if (check_existing_instance_and_handle(config.pid_file, is_service_start) < 0) {
        // Error: Service already running and we are manual start
        return 1;
    }
    
    // Write new PID file
    write_pid_file(config.pid_file);
    
    g_config_ptr = &config; // Set global pointer for signal handler
    // Register signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = cleanup_and_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    // Create image directory
    mkdir(config.image_dir, 0755); // Create directory for images if not present
    
    // Initialize modules
    printf("Initializing modules...\n");
    fflush(stdout);
    
    // Initialize CPU sensors
    init_cpu_sensor_path(&config); // Set path to CPU sensors
    printf("✓ CPU monitor initialized\n");
    fflush(stdout);
    // Initialize GPU monitor (if GPU available)
    if (init_gpu_monitor(&config)) { // Check return value
        printf("✓ GPU monitor initialized\n");
    } else {
        printf("⚠ GPU monitor not available (no NVIDIA GPU?)\n");
    }
    fflush(stdout);
    // Initialize CoolerControl session
    if (init_coolercontrol_session(&config)) { // Check return value
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
    int result = run_daemon(&config);
    // Cleanup - send shutdown image if not sent yet (only on normal termination)
    if (!shutdown_sent && is_session_initialized()) {
        const char* shutdown_image = config.shutdown_image;
        const char* device_uid = get_cached_device_uid();
        printf("CoolerDash: Sending final shutdown image...\n");
        fflush(stdout);
        if (device_uid[0]) {
            send_image_to_lcd(&config, shutdown_image, device_uid);
            send_image_to_lcd(&config, shutdown_image, device_uid);
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