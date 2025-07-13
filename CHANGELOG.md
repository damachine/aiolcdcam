# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project follows a custom versioning scheme: **1.year.month.day.hourminute**
(e.g., 1.25.07.08.1759 = Year 2025, July 8th, 17:59)

---

## [1.25.07.13.XXXX] - 2025-07-13

### Added
- **Persistent UUID Caching System**: After first-run automatic detection, UUID is cached to `/var/cache/aiolcdcam/device.uuid` for fast startup
- **Automatic AIO Device Detection**: Runtime UUID detection via CoolerControl API - no manual configuration required
- **UUID Cache Validation**: On startup, cached UUID is validated against current device list to ensure compatibility
- **Memory UUID Cache**: Detected UUID is cached in memory to minimize CoolerControl API calls during runtime
- **Millisecond-precision GPU cache interval**: GPU_CACHE_INTERVAL now supports float values (e.g. 2.5f) for accurate caching
- **Helper function get_current_time_ms()** for precise time measurement
- **4-stage temperature color gradient documentation**: Clear thresholds (≤55°C Green, ≤65°C Orange, ≤75°C Hot Orange, >75°C Red)

### Changed
- **Removed manual UUID configuration**: Device UUID is now automatically detected at runtime using CoolerControl API
- **All GPU monitoring functions** (read_gpu_temp, get_gpu_usage_data, get_gpu_data_full) now use millisecond-based caching
- **Display layout improvements**: Font sizes increased and horizontal bars made smaller for better readability
- **Degree symbol (°) positioning** optimized for CPU, GPU, and coolant temperature displays
- **Documentation overhaul**: All references to manual UUID configuration removed from README, install scripts, and manpages
- **Installation process simplified**: No more manual config.h editing required before installation
- **Terminology consistency**: All "Kraken" references replaced with neutral "AIO" or "AIO LCD" terminology
- **Image filename standardized**: All references now use `aiolcdcam.png` consistently

### Fixed
- **GPU cache interval bug**: Now truly supports 2.5 seconds with millisecond precision (was rounding to 2 seconds)
- **Function declaration order** in gpu_monitor.c for successful compilation
- **Automatic device detection**: Robust UUID detection using `"type": "Liquidctl"` as reliable indicator

### Removed
- **Manual UUID configuration requirement**: No longer need to edit config.h before installation
- **Static AIO_UUID define**: Replaced with runtime detection system
- **Hardware-specific output**: Generic "AIO LCD" terminology instead of device-specific references

### Technical
- **Persistent UUID Caching**: First-run detection saves UUID to `/var/cache/aiolcdcam/device.uuid` for instant subsequent startups
- **Dynamic UUID detection system**: Queries CoolerControl API and caches detected UUID
- **Cache validation on startup**: Ensures cached UUID still exists in current device list before use
- **get_cached_aio_uuid() function**: Provides runtime-detected UUID throughout application with memory caching
- **Automatic compatibility**: Works with all CoolerControl-compatible AIO devices with LCD displays
- **Cache directory management**: Makefile and PKGBUILD handle `/var/cache/aiolcdcam/` creation and cleanup
- **systemd service permissions**: ReadWritePaths configured for cache directory access
- **All changes committed and tested**: Ready for production use

---

## [1.25.07.08.2234] - 2025-07-08

### Added
- **Beta Status Clarification**: Comprehensive beta warnings and user experience roadmap
  - Prominent beta warning section in README.md with current limitations
  - User experience improvement roadmap with planned features
  - Clear indication that manual configuration and technical knowledge required
  - Future GUI configuration interface and one-click installer planned
  - System tray integration and theme system in development
  - Screenshot of current LCD output on NZXT Kraken 2023
- **GitHub Release Integration**: Complete GitHub Releases workflow
  - create-release.sh script for automated release tarball generation
  - PKGBUILD configured to use GitHub Releases tarball as source
  - Correct SHA256 checksum updated after GitHub upload
  - .gitignore created to exclude build artifacts and release files
- **PKGBUILD Auto-Cleanup**: Automatic detection and removal of conflicting manual installations
  - Intelligent detection of existing Makefile installations
  - Automatic execution of `sudo make uninstall` for clean removal
  - Fallback manual cleanup if make uninstall fails
  - Detailed progress messages and conflict resolution
  - Prevents installation conflicts between Makefile and PKGBUILD methods

### Changed
- **Beta Documentation**: Updated all documentation to reflect early development status
  - README.md: Added beta warning with development stage information
  - PKGBUILD: Added (BETA) tag in package description
  - AUR-README.md: Added beta warning and removed premature AUR publication instructions
  - Clear indication this is tested only on developer's NZXT Kraken 2023 system
- **AUR Publication Status**: Clarified NOT YET PUBLISHED status
  - Removed premature AUR publication instructions from AUR-README.md
  - Added comprehensive checklist for future AUR publication requirements
  - Emphasized current focus on development and testing phase
  - Added "Help Wanted" section for community testing
- **Version Synchronization**: Updated to 1.25.07.08.2234 across all files
  - Synchronized version across Makefile, PKGBUILD, .SRCINFO, manpage
  - Maintained consistency in timestamp-based versioning scheme
- **UUID Configuration Documentation**: Enhanced clarity on UUID setup requirements
  - Emphasized UUID must be configured BEFORE building (not after installation)
  - Updated both Arch Linux and manual installation instructions
  - Added explicit warnings in PKGBUILD prepare() function
  - Clear step-by-step order for proper configuration

### Technical
- **Release Automation**: create-release.sh script for consistent GitHub Releases
- **Build Artifact Management**: .gitignore excludes release tarballs and build files
- **Package Conflict Resolution**: Enhanced PKGBUILD prepare() function with automatic cleanup
- **Development Status**: Clear beta labeling across all user-facing documentation
- **Source Distribution**: GitHub Releases tarball as canonical source for PKGBUILD

### Removed
- **Premature AUR Instructions**: Removed "Publishing to AUR" section from AUR-README.md
- **Test PKGBUILD Files**: Cleaned up PKGBUILD-test, PKGBUILD-local, PKGBUILD-github-test files
- **Redundant Documentation**: Eliminated duplicate setup instructions across files

---

## [1.25.07.08.1759] - 2025-07-08

### Changed
- **Versioning Scheme**: Switched from semantic versioning to timestamp-based versioning
  - New format: 1.year.month.day.hourminute (e.g., 1.25.07.08.1759)
  - Updated Makefile, PKGBUILD, and all documentation
  - Consistent version propagation across all build and configuration files
- **Documentation**: Updated version references throughout all documentation

### Technical
- Version string format updated in Makefile with explanatory comment
- PKGBUILD version updated for AUR consistency
- All legacy version references standardized to new scheme

---

## [0.1.3] - 2025-01-26

### Added
- **Arch Linux PKGBUILD**: Complete packaging for Arch User Repository (AUR)
  - PKGBUILD with automatic dependency management
  - aiolcdcam.install with post-installation instructions
  - .SRCINFO metadata file for AUR publication
  - PKGBUILD-local for local testing without source download
  - Installation to /opt/aiolcdcam/ with symlink to /usr/bin/aiolcdcam
- **C99 Compliance**: Complete C99 standard conformity with explicit feature test macros
- **Display Mode Instructions**: Detailed instructions for switching between systemd and manual operation
- **Standardized Include Paths**: All C source files use unified `../include/` paths
- **English Documentation**: Complete translation of all documentation to English
- **English Code Comments**: All source code comments and descriptions translated to English
- **MIT License**: Added MIT license file for open source distribution
- **Donation Support**: Added cryptocurrency donation support (Bitcoin and Dogecoin)
  - GitHub FUNDING.yml for integrated donation links
  - README section with wallet addresses and support options

### Changed
- **Documentation Overhaul**: Complete restructuring of README.md and man page
  - Clear order: Title → Description → Features → Installation → Configuration → Usage → Troubleshooting
  - Removal of redundant and legacy content
  - Clarification of UUID configuration before installation
  - Added Arch Linux installation section with PKGBUILD and AUR options
- **Installation Process**: Emphasis that config.h UUID must be set before `make install`
- **Code Structure**: All C files standardized for strict C99 compliance
- **Makefile**: Explicit `-std=c99` flag for C99 compliance
- **Configuration**: Improved comments in config.h for UUID setup
- **Source Code**: Complete translation of all German comments to English in all source files
  - config.h: All configuration comments and setup instructions
  - main.c: All function comments and documentation
  - display.c: All rendering and display logic comments  
  - cpu_monitor.c: All sensor monitoring comments
  - gpu_monitor.c: All GPU monitoring and caching comments
  - coolant_monitor.c: All coolant temperature monitoring comments
  - coolercontrol.c: All HTTP session and API comments
  - All header files: Function documentation and variable descriptions
  - Makefile: All comments, variable names, and user messages
  - Complete consistency across entire codebase

### Fixed
- **POSIX Compliance**: Feature test macros (_POSIX_C_SOURCE, _XOPEN_SOURCE) correctly set
- **usleep Function**: Correct handling of usleep function for C99/POSIX
- **Build Process**: All compilation warnings and errors for strict C99 resolved
- **Man Page Structure**: Man page structure now matches README structure

### Removed
- **Legacy Content**: Removal of outdated build references and redundant documentation
- **German Documentation**: Complete migration to English documentation
- **Duplicate Instructions**: Elimination of duplicate setup instructions

### Technical
- **Package Structure**: Professional Arch Linux packaging following AUR standards
- **Binary Installation**: /opt/aiolcdcam/bin/aiolcdcam with symlink to /usr/bin/aiolcdcam
- **Source Code Availability**: Complete source code included for post-installation UUID configuration
- **Dependency Management**: Automatic detection and installation of runtime dependencies

---

## [0.1.2] - 2025-07-08

### Added
- **CoolerControl Setup Instructions**: Detailed guide for correct LCD configuration
- **LCD Image Mode Requirement**: Explicit note that LCD must be set to "Image" mode
- **Extended Documentation**: Complete setup steps in README and man page
- **Man Page Improvements**: SETUP section with step-by-step guide

### Changed
- **Binary Name**: From `nzxt` to `aiolcdcam` for better uniqueness
- **Service Name**: Consistent naming as `aiolcdcam.service`
- **PID File**: From `/var/run/nzxt_cam.pid` to `/var/run/aiolcdcam.pid`
- **Documentation**: All examples and references updated to `aiolcdcam`
- **Man Page**: Corrected service names and binary references

### Fixed
- **Consistent Naming**: Unified use of `aiolcdcam` in all files
- **Setup Documentation**: Clarification of CoolerControl configuration steps
- **Service References**: Correct systemd service names in documentation

---

## [0.1.1] - 2025-07-06

### Added
- **Single-Instance Enforcement**: Only one aiolcdcam daemon instance can run simultaneously
- **Robust PID File Management**: `/var/run/aiolcdcam.pid` for systemd-compatible process management
- **Intelligent Instance Coordination**: Service vs. manual starts are handled correctly
- **Shutdown Image in C**: Direct sending of face.png image on daemon stop
- **Makefile Process Management**: Automatic termination of all running `aiolcdcam` processes before installation
- **Complete English Output**: All user-facing messages in English
- **Extended Error Handling**: EPERM handling for root-owned processes
- **🆕 Automatic Dependency Installation**: `make install` automatically detects Linux distribution and installs missing dependencies
- **🆕 Distribution Auto-Detection**: Support for Arch, Debian/Ubuntu, Fedora, RHEL/CentOS, openSUSE
- **🆕 One-Command Installation**: Complete installation with `sudo make install` (Dependencies + Build + Service Setup)
- **🆕 Universal Compatibility**: Works out-of-the-box on all major Linux distributions
- **🆕 Robust Package Installation**: Automatic fallback information for unknown distributions

### Changed
- **PID File Location**: From `/tmp/aiolcdcam.pid` to `/var/run/aiolcdcam.pid` (systemd-compatible)
- **Signal Handler**: Integrated shutdown image sending directly in C
- **Makefile**: Extended service and process detection with `killall`
- **Daemon Output**: Complete translation of German output to English
- **Systemd Service**: Simplified service file without ExecStop (shutdown handled in C)
- **🆕 Makefile Install Target**: Now with automatic dependency checking and installation
- **🆕 Help Output**: Cleaned `make help` without manual dependency targets (handled internally)
- **🆕 User Friendliness**: One command for complete installation instead of multiple steps
- **🆕 Distribution Agnostic**: No manual dependency installation steps needed anymore
- **🆕 Makefile Structure**: Internal targets for clean user API

### Fixed
- **Duplicate Instances**: Prevents multiple simultaneous daemon execution
- **Service Conflicts**: Manual start fails if systemd service is running
- **Zombie Processes**: Robust termination of hanging instances
- **Shutdown Race Conditions**: `shutdown_sent` flag prevents duplicate image transmission
- **Permission Errors**: Correct handling of EPERM in process checks

### Security
- **Process Isolation**: Single daemon instance prevents resource conflicts
- **PID File Security**: Secure PID management in `/var/run/`

---

## [0.1.0] - 2025-07-05

### Added
- **Modular C Architecture**: Complete split into separate modules
  - `cpu_monitor.c/.h` - CPU temperature and load monitoring
  - `gpu_monitor.c/.h` - GPU temperature and load monitoring (NVIDIA)
  - `coolant_monitor.c/.h` - Coolant temperature monitoring
  - `display.c/.h` - Rendering engine and mode logic
  - `coolercontrol.c/.h` - REST API communication
- **Central Configuration**: All settings in `include/config.h`
- **4 Display Modes**:
  - `def` - Temperatures only (resource-efficient)
  - `1` - Temperatures + vertical load bars
  - `2` - Temperatures + circular diagrams
  - `3` - Temperatures + horizontal load bars
- **I/O Optimization**: Mode-specific sensor data querying
- **Change Detection**: Display updates only on significant changes
- **Professional systemd Integration**: Complete service support
- **Comprehensive Build System**: Modular Makefile with dependency detection

### Changed
- **Project Structure**: From monolith to modular standard C architecture
- **Performance**: Intelligent caching and selective sensor querying
- **Maintainability**: Clean separation of logic and configuration

### Fixed
- **Memory Leaks**: Complete Cairo resource management
- **Performance Bottlenecks**: Optimized rendering pipeline
- **Code Duplication**: Elimination of redundant functions

---

## [0.0.1] - 2025-07-04

### Added
- **First Working Version**: Monolithic C program `aiolcdcam.c`
- **NZXT Kraken LCD Support**: Display of CPU/GPU temperatures
- **CoolerControl Integration**: REST API communication via curl
- **Cairo-based Rendering**: PNG generation for LCD display
- **Basic Modes**: Temperature-only and load displays
- **hwmon Integration**: Automatic CPU/coolant sensor detection
- **nvidia-smi Integration**: GPU temperature and load data

### Technical Details
- **Language**: C99 with GCC optimizations
- **Dependencies**: libcairo, libcurl, nvidia-smi
- **Target**: Linux systems with NZXT Kraken AIO coolers
- **Display**: 240x240 pixel LCD with PNG upload

---

## Planned Features (Roadmap)

### [0.2.0] - Planned
- **Independence from CoolerControl**: Direct USB/HID communication with NZXT devices
- **Native NZXT Protocol**: Implementation of native NZXT Kraken protocol
- **Standalone Operation**: Complete elimination of CoolerControl dependency
- **Multi-Device Support**: Support for multiple NZXT devices
- **Configuration Files**: YAML/JSON-based configuration
- **Extended Modes**: More visualization options
- **Logging System**: Structured logging with log levels

### [0.3.0] - Planned
- **Plugin Architecture**: Extensible sensor modules
- **libusb Integration**: Direct USB communication without daemon dependencies
- **Reverse Engineering**: Complete NZXT protocol documentation
- **Web Interface**: Browser-based configuration
- **Alerts and Thresholds**: Temperature notifications
- **Historical Data**: Temperature logging and trends
- **Performance Profiling**: Detailed performance metrics

### [0.4.0] - Long-term
- **Universal NZXT Support**: Kraken, Smart Device V2, RGB & Fan Controller
- **Community Protocol**: Open-source NZXT protocol library
- **Cross-Platform**: Windows and macOS support
- **Hardware Detection**: Automatic NZXT device detection

---

## Development Guidelines

- **Semantic Versioning**: MAJOR.MINOR.PATCH format
- **Keep a Changelog**: Structured change documentation
- **C99 Standard**: Modern C development with GCC optimizations
- **Modular Architecture**: Clean separation of functionalities
- **Performance First**: Resource-efficient implementation
- **Production Ready**: Robust error handling and systemd integration

---

**Author**: DAMACHINE (christkue79@gmail.com)  
**License**: MIT License  
**Repository**: LCD AIO CAM - Modular C Daemon
