# AUR Package Instructions for CoolerDash

## ⚡ Recommended Installation (Arch Linux)

CoolerDash is best installed via the AUR using `makepkg -si` for full dependency management and automatic conflict cleanup.

### Quick Install (AUR/Local Build)

```bash
# 1. Clone the repository
 git clone https://github.com/damachine/coolerdash.git
 cd coolerdash

# 2. Start CoolerControl daemon (required)
 sudo systemctl start coolercontrold

# 3. Build and install with all dependencies and cleanup
 makepkg -si

# 4. Enable and start CoolerDash service
 sudo systemctl enable --now coolerdash.service

# 5. Check service status
 sudo systemctl status coolerdash.service
 journalctl -xeu coolerdash.service
```

### Alternative: Local .pkg.tar.zst Installation

```bash
# 1. Build the package (creates .pkg.tar.zst)
 makepkg -s

# 2. Install the package manually
 sudo pacman -U coolerdash-*.pkg.tar.zst

# 3. Enable and start service
 sudo systemctl enable --now coolerdash.service
```

---

## 📦 Package Details
- Installs to `/opt/coolerdash/` (binary, images, docs)
- Symlink in `/usr/bin/coolerdash` for global access
- Systemd service: `coolerdash.service` (auto-start supported)
- No cache or UID files are created or left after uninstall
- All files and service units are removed on uninstall

## 🖥️ LCD & CoolerControl Requirements
- CoolerControl daemon must be running: `sudo systemctl start coolercontrold`
- LCD must be set to "Image/gif" mode in CoolerControl GUI
- Compatible with NZXT Kraken (Z/X-Series) and other LCD AIOs supported by CoolerControl

## 🛠️ Configuration
- No config file needed. All main options are in `include/config.h` (edit before build if needed)
- Important values: display size, refresh interval, temperature thresholds, font, image paths

## 🧹 Uninstallation
- Removes all files from `/opt/coolerdash/` and disables the service
- No cache or UID files remain after uninstall

## 🐧 Arch Linux Specific Notes
- Follows Arch packaging standards for third-party software
- Automatic detection and removal of manual installations (see PKGBUILD)
- All dependencies are managed via PKGBUILD and Makefile

## 📚 Documentation & Support
- Main documentation: [README.md](README.md)
- Man page: `man coolerdash`
- For issues, open a ticket at https://github.com/damachine/coolerdash/issues

---

**Developed and tested on Arch Linux. For other distributions, see the main README.md.**
