# AUR Package Instructions

## ⚠️ **BETA SOFTWARE WARNING**

**This package is in early development stage!**
- Currently tested only on developer's NZXT Kraken 2023
- Features may change or break without notice
- Use at your own risk and report issues

![LCD Display Output](images/coolerdash.png)
*Current LCD output on NZXT Kraken 2023*

## Files for AUR Publication

This directory contains the necessary files for **future** publishing to the Arch User Repository (AUR):

### Core Files:
- `PKGBUILD` - Main package build script for AUR
- `coolerdash.install` - Post-installation scripts and user instructions
- `.SRCINFO` - Generated metadata file for AUR (auto-generated from PKGBUILD)

## 🚧 **Development & Testing Phase**

**Current Status: NOT YET PUBLISHED TO AUR**

This package is currently in development and testing phase. Before AUR publication, extensive testing is required on multiple systems and hardware configurations.

### Local Testing Only

**For developers and testers only - NOT for general use yet!**

```bash
# Test the PKGBUILD locally
git clone https://github.com/damachine/coolerdash.git
cd coolerdash

# STEP 1: Start CoolerControl daemon (UUID auto-detected)
sudo systemctl start coolercontrold

# STEP 2: Test build process
makepkg -s

# STEP 3: Test installation (local only)
sudo pacman -U coolerdash-1.25.07.15.1300-1-x86_64.pkg.tar.zst

# STEP 4: Test functionality
coolerdash --help
sudo systemctl start coolerdash.service
sudo systemctl status coolerdash.service
```

## 📋 **Future AUR Publication Checklist**

Before publishing to AUR, the following requirements must be met:

- [ ] **Multi-System Testing**: Test on various hardware configurations
- [ ] **Extended Hardware Support**: Test with different NZXT Kraken models  
- [ ] **Stability Testing**: Long-term stability and reliability verification
- [ ] **Documentation**: Complete user documentation and troubleshooting guides
- [ ] **Issue Resolution**: Address major bugs and compatibility issues
- [ ] **Community Feedback**: Gather feedback from beta testers
- [ ] **Code Review**: Final code review and optimization

### Future AUR Workflow

**When ready for AUR publication (NOT NOW):**

```bash
# Clone AUR repository (future)
git clone ssh://aur@aur.archlinux.org/coolerdash.git aur-coolerdash
cd aur-coolerdash

# Copy files from main project
cp ../coolerdash/PKGBUILD .
cp ../coolerdash/coolerdash.install .

# Generate .SRCINFO
makepkg --printsrcinfo > .SRCINFO

# Update checksums for release
makepkg -g

# Publish to AUR
git add PKGBUILD coolerdash.install .SRCINFO
git commit -m "Initial release - CoolerDash LCD AIO daemon for NZXT Kraken"
git push origin master
```

## 🧪 **Current Testing Phase**

**This project is currently in beta testing phase:**

- **Hardware Testing**: Currently tested only on developer's NZXT Kraken 2023
- **Need Testers**: Looking for volunteers with different NZXT Kraken models
- **Bug Reports**: Issues expected and should be reported on GitHub
- **Feature Development**: Core features still being developed and refined

**Help Wanted:**
- Test on different NZXT Kraken models (X-Series, Z-Series)
- Report compatibility issues
- Suggest features and improvements
- Documentation feedback

## Package Architecture
- **Runtime**: cairo, libcurl-gnutls, coolercontrol
- **Build**: gcc, make, pkg-config
- **Optional**: nvidia-utils (GPU monitoring), lm_sensors (additional sensors)

### Installation:
- **Binary**: `/opt/coolerdash/bin/coolerdash` + symlink to `/usr/bin/coolerdash`
- **Source**: Complete source code in `/opt/coolerdash/` for UUID reconfiguration
- **Service**: systemd service file for automatic startup
- **Documentation**: man page, README, changelog

### Post-Installation:
- **Automatic Cleanup**: PKGBUILD automatically detects and removes conflicting manual installations
- **UUID Configuration**: Automatic instructions for device UUID setup
- **CoolerControl Integration**: Dependency verification and setup guidance
- **Service Management**: systemd service setup and upgrade handling
- **Configuration Preservation**: Source code included for post-installation UUID reconfiguration

## Local Testing Instructions

**IMPORTANT: Start CoolerControl BEFORE building the package!**

```bash
# 1. FIRST: Start CoolerControl daemon
sudo systemctl start coolercontrold

# 2. THEN: Build and install the package (with automatic cleanup)
makepkg -si

# The build process will:
# 1. Detect existing manual installations
# 2. Automatically run 'sudo make uninstall' 
# 3. Clean up any conflicts
# 4. Proceed with PKGBUILD installation
# 5. Automatically detect AIO device UUID at runtime
```

## Automatic Conflict Resolution

The PKGBUILD now includes intelligent conflict detection:

- **Detection**: Automatically finds manual installations via Makefile
- **Cleanup**: Runs `sudo make uninstall` to cleanly remove conflicts
- **Fallback**: Manual cleanup if make uninstall fails
- **Transparency**: Detailed progress messages during cleanup
- **Safety**: Only removes conflicting installations, preserves user data

## Notes

- The package installs to `/opt/coolerdash/` following Arch Linux standards for third-party software
- A symlink in `/usr/bin/coolerdash` provides system-wide access
- Source code is included for post-installation UUID configuration
- The package properly handles systemd service management and cleanup

## Images

- `shutdown.png`: The image displayed on the LCD when the service is stopped or shut down. The path is configurable via the `SHUTDOWN_IMAGE_PATH` macro in `config.h`.
- `coolerdash.png`: The main sensor/status image, created at runtime.

## Installation

- Installs to `/opt/coolerdash/bin/` and `/opt/coolerdash/images/`.
- No cache directories or UID files are created or used.
- The shutdown image is always `/opt/coolerdash/images/shutdown.png` (configurable in code).

## Uninstallation

- Removes all files from `/opt/coolerdash/` and disables the service.
- No cache or UID files remain after uninstall.
