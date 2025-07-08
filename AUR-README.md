# AUR Package Instructions

## Files for AUR Publication

This directory contains the necessary files for publishing LCD AIO CAM to the Arch User Repository (AUR):

### Core Files:
- `PKGBUILD` - Main package build script for AUR
- `aiolcdcam.install` - Post-installation scripts and user instructions
- `.SRCINFO` - Generated metadata file for AUR (auto-generated from PKGBUILD)

### Local Testing:
- `PKGBUILD-local` - Modified PKGBUILD for local testing without downloading sources

## Publishing to AUR

### 1. Prepare AUR Repository
```bash
# Clone AUR repository (empty at first)
git clone ssh://aur@aur.archlinux.org/aiolcdcam.git aur-aiolcdcam
cd aur-aiolcdcam

# Copy files from main project
cp ../aiolcdcam/PKGBUILD .
cp ../aiolcdcam/aiolcdcam.install .

# Generate .SRCINFO
makepkg --printsrcinfo > .SRCINFO
```

### 2. Update Version and Checksums
```bash
# For new releases, update PKGBUILD:
# - pkgver=X.X.X
# - sha256sums=(run 'makepkg -g' to generate)

# Generate new checksums
makepkg -g
```

### 3. Test Package Locally
```bash
# Test build process
makepkg -s

# Test installation
sudo pacman -U aiolcdcam-X.X.X-1-x86_64.pkg.tar.zst

# Test functionality
aiolcdcam --help
sudo systemctl status aiolcdcam.service
```

### 4. Publish to AUR
```bash
# Add all files
git add PKGBUILD aiolcdcam.install .SRCINFO

# Commit with meaningful message
git commit -m "Initial release v0.1.3 - LCD AIO CAM daemon for NZXT Kraken"

# Push to AUR
git push origin master
```

## Package Features

### Dependencies:
- **Runtime**: cairo, libcurl-gnutls, coolercontrol
- **Build**: gcc, make, pkg-config
- **Optional**: nvidia-utils (GPU monitoring), lm_sensors (additional sensors)

### Installation:
- **Binary**: `/opt/aiolcdcam/bin/aiolcdcam` + symlink to `/usr/bin/aiolcdcam`
- **Source**: Complete source code in `/opt/aiolcdcam/` for UUID reconfiguration
- **Service**: systemd service file for automatic startup
- **Documentation**: man page, README, changelog

### Post-Installation:
- Automatic UUID configuration instructions
- CoolerControl dependency verification
- Service setup guidance
- Upgrade handling with configuration preservation

## Local Testing Instructions

### Using PKGBUILD-local:
```bash
# Test local build
makepkg -p PKGBUILD-local -s

# Install locally built package
sudo pacman -U aiolcdcam-0.1.3-1-x86_64.pkg.tar.zst
```

## Notes

- The package installs to `/opt/aiolcdcam/` following Arch Linux standards for third-party software
- A symlink in `/usr/bin/aiolcdcam` provides system-wide access
- Source code is included for post-installation UUID configuration
- The package properly handles systemd service management and cleanup
