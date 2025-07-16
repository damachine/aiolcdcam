# Makefile for CoolerDash (Standard C Project Structure)
# CoolerDash - Modular C daemon for Liquid Cooler LCD Dashboard via CoolerControl (BETA)

# ================================
# ⚠️  BETA SOFTWARE - EARLY DEVELOPMENT STAGE
# ================================
# This software is in very early development and has been tested
# only on the developer's system with NZXT AIO Kraken 2023.
#
# - Features may change or break without notice
# - Bugs and issues are expected
# - Use at your own risk
# - Please report issues to: https://github.com/damachine/coolerdash/issues
# ================================

# ================================
# Manual Installation Check
# ================================
# If you see errors about "conflicting files" or manual installation,
# run 'sudo make uninstall' and remove leftover files in /opt/coolerdash,
# /usr/bin/coolerdash, /etc/systemd/system/coolerdash.service.
# ================================

# Version (Format: 0.year.month.day.hourminute)
VERSION = 0.2025.07.16.0000

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -march=x86-64-v3 -Iinclude $(shell pkg-config --cflags cairo)
LIBS = $(shell pkg-config --libs cairo) -lcurl -lm
TARGET = coolerdash

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = build
BINDIR = bin

# Source code files
MAIN_SOURCE = $(SRCDIR)/main.c
MODULES = $(SRCDIR)/cpu_monitor.c $(SRCDIR)/gpu_monitor.c $(SRCDIR)/coolant_monitor.c $(SRCDIR)/display.c $(SRCDIR)/coolercontrol.c
HEADERS = $(INCDIR)/config.h $(INCDIR)/cpu_monitor.h $(INCDIR)/gpu_monitor.h $(INCDIR)/coolant_monitor.h $(INCDIR)/display.h $(INCDIR)/coolercontrol.h
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(MODULES))
ALL_SOURCES = $(MAIN_SOURCE) $(MODULES)

SERVICE = systemd/coolerdash.service
MANPAGE = man/coolerdash.1
README = README.md

# Colors for terminal output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[1;37m
RESET = \033[0m

# Icons (Unicode)
ICON_BUILD = 🔨
ICON_INSTALL = 📦
ICON_SERVICE = ⚙️
ICON_SUCCESS = ✅
ICON_WARNING = ⚠️
ICON_INFO = ℹ️
ICON_CLEAN = 🧹
ICON_UNINSTALL = 🗑️

# Standard Build Target - Standard C project structure
$(TARGET): $(OBJDIR) $(BINDIR) $(OBJECTS) $(MAIN_SOURCE)
	@printf "\n$(YELLOW)═══════════════════════════════════════════════════════$(RESET)\n"
	@printf "$(RED)⚠️  BETA SOFTWARE - EARLY DEVELOPMENT STAGE$(RESET)\n"
	@printf "$(YELLOW)═══════════════════════════════════════════════════════$(RESET)\n"
	@printf "This software is in very early development and has been tested only on the developer's system with NZXT AIO Kraken 2023.\n"
	@printf " - Features may change or break without notice\n"
	@printf " - Bugs and issues are expected\n"
	@printf " - Use at your own risk\n"
	@printf " - Please report issues to: https://github.com/damachine/coolerdash/issues\n"
	@printf "$(YELLOW)═══════════════════════════════════════════════════════$(RESET)\n\n"
	@printf "$(PURPLE)Manual Installation Check:$(RESET)\n"
	@printf "If you see errors about 'conflicting files' or manual installation, run 'sudo make uninstall' and remove leftover files in /opt/coolerdash, /usr/bin/coolerdash, /etc/systemd/system/coolerdash.service.\n\n"
	@printf "$(ICON_BUILD) $(CYAN)Compiling $(TARGET) (Standard C structure)...$(RESET)\n"
	@printf "$(BLUE)Structure:$(RESET) src/ include/ build/ bin/\n"
	@printf "$(BLUE)CFLAGS:$(RESET) $(CFLAGS)\n"
	@printf "$(BLUE)LIBS:$(RESET) $(LIBS)\n"
	$(CC) $(CFLAGS) -o $(BINDIR)/$(TARGET) $(MAIN_SOURCE) $(OBJECTS) $(LIBS)
	@printf "$(ICON_SUCCESS) $(GREEN)Build successful: $(BINDIR)/$(TARGET)$(RESET)\n"

# Create build directory
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Create bin directory
$(BINDIR):
	@mkdir -p $(BINDIR)

# Compile object files (with correct paths)
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/%.h $(INCDIR)/config.h | $(OBJDIR)
	@printf "$(ICON_BUILD) $(YELLOW)Compiling module: $<$(RESET)\n"
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies for header changes
$(OBJECTS): $(HEADERS)

# Clean Target
clean:
	@printf "$(ICON_CLEAN) $(YELLOW)Cleaning up...$(RESET)\n"
	rm -f $(BINDIR)/$(TARGET) $(OBJECTS) *.o
	rm -rf $(OBJDIR) $(BINDIR)
	@printf "$(ICON_SUCCESS) $(GREEN)Cleanup completed$(RESET)\n"

# Detect Linux Distribution (internal function)
detect-distro:
	@if [ -f /etc/arch-release ]; then \
		echo "arch"; \
	elif [ -f /etc/debian_version ]; then \
		echo "debian"; \
	elif [ -f /etc/fedora-release ]; then \
		echo "fedora"; \
	elif [ -f /etc/redhat-release ]; then \
		echo "rhel"; \
	elif [ -f /etc/opensuse-release ]; then \
		echo "opensuse"; \
	else \
		echo "unknown"; \
	fi

# Install Dependencies (internal function)
install-deps:
	@DISTRO=$$($(MAKE) detect-distro); \
	case $$DISTRO in \
		arch) \
			printf "$(ICON_INSTALL) $(GREEN)Installing dependencies for Arch Linux/Manjaro...$(RESET)\n"; \
			sudo pacman -S --needed cairo libcurl-gnutls gcc make pkg-config ttf-roboto || { \
				printf "$(ICON_WARNING) $(RED)Error installing dependencies!$(RESET)\n"; \
				printf "$(YELLOW)Please run manually:$(RESET) sudo pacman -S cairo libcurl-gnutls gcc make pkg-config ttf-roboto\n"; \
				exit 1; \
			}; \
			;; \
		debian) \
			printf "$(ICON_INSTALL) $(GREEN)Installing dependencies for Ubuntu/Debian...$(RESET)\n"; \
			sudo apt update && sudo apt install -y libcairo2-dev libcurl4-openssl-dev gcc make pkg-config fonts-roboto || { \
				printf "$(ICON_WARNING) $(RED)Error installing dependencies!$(RESET)\n"; \
				printf "$(YELLOW)Please run manually:$(RESET) sudo apt install libcairo2-dev libcurl4-openssl-dev gcc make pkg-config fonts-roboto\n"; \
				exit 1; \
			}; \
			;; \
		fedora) \
			printf "$(ICON_INSTALL) $(GREEN)Installing dependencies for Fedora...$(RESET)\n"; \
			sudo dnf install -y cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts || { \
				printf "$(ICON_WARNING) $(RED)Error installing dependencies!$(RESET)\n"; \
				printf "$(YELLOW)Please run manually:$(RESET) sudo dnf install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
				exit 1; \
			}; \
			;; \
		rhel) \
			printf "$(ICON_INSTALL) $(GREEN)Installing dependencies for RHEL/CentOS...$(RESET)\n"; \
			sudo yum install -y cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts || { \
				printf "$(ICON_WARNING) $(RED)Error installing dependencies!$(RESET)\n"; \
				printf "$(YELLOW)Please run manually:$(RESET) sudo yum install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
				exit 1; \
			}; \
			;; \
		opensuse) \
			printf "$(ICON_INSTALL) $(GREEN)Installing dependencies for openSUSE...$(RESET)\n"; \
			sudo zypper install -y cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts || { \
				printf "$(ICON_WARNING) $(RED)Error installing dependencies!$(RESET)\n"; \
				printf "$(YELLOW)Please run manually:$(RESET) sudo zypper install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
				exit 1; \
			}; \
			;; \
		*) \
			printf "$(ICON_WARNING) $(RED)Unknown distribution detected!$(RESET)\n"; \
			printf "\n"; \
			printf "$(YELLOW)Please install the following dependencies manually:$(RESET)\n"; \
			printf "\n"; \
			printf "$(WHITE)Arch Linux / Manjaro:$(RESET)\n"; \
			printf "  sudo pacman -S cairo libcurl-gnutls gcc make pkg-config ttf-roboto\n"; \
			printf "\n"; \
			printf "$(WHITE)Ubuntu / Debian:$(RESET)\n"; \
			printf "  sudo apt install libcairo2-dev libcurl4-openssl-dev gcc make pkg-config fonts-roboto\n"; \
			printf "\n"; \
			printf "$(WHITE)Fedora:$(RESET)\n"; \
			printf "  sudo dnf install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
			printf "\n"; \
			printf "$(WHITE)RHEL / CentOS:$(RESET)\n"; \
			printf "  sudo yum install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
			printf "\n"; \
			printf "$(WHITE)openSUSE:$(RESET)\n"; \
			printf "  sudo zypper install cairo-devel libcurl-devel gcc make pkg-config google-roboto-fonts\n"; \
			printf "\n"; \
			exit 1; \
			;; \
	esac

# Check Dependencies for Installation (internal function called by install)
check-deps-for-install:
	@MISSING=""; \
	if ! pkg-config --exists cairo >/dev/null 2>&1; then \
		MISSING="$$MISSING cairo"; \
	fi; \
	if ! pkg-config --exists libcurl >/dev/null 2>&1; then \
		MISSING="$$MISSING libcurl"; \
	fi; \
	if ! command -v gcc >/dev/null 2>&1; then \
		MISSING="$$MISSING gcc"; \
	fi; \
	if ! command -v make >/dev/null 2>&1; then \
		MISSING="$$MISSING make"; \
	fi; \
	if [ -n "$$MISSING" ]; then \
		printf "$(ICON_WARNING) $(YELLOW)Missing dependencies detected:$$MISSING$(RESET)\n"; \
		printf "$(ICON_INSTALL) $(CYAN)Auto-installing dependencies...$(RESET)\n"; \
		$(MAKE) install-deps || { \
			printf "$(ICON_WARNING) $(RED)Auto-installation failed!$(RESET)\n"; \
			printf "$(YELLOW)Please install dependencies manually before running 'make install'$(RESET)\n"; \
			exit 1; \
		}; \
	fi

# Install Target - Installs to /opt/coolerdash/ (with automatic dependency check and service management)
install: check-deps-for-install $(TARGET)
	@printf "\n"
	@printf "$(ICON_INSTALL) $(WHITE)═══ COOLERDASH INSTALLATION ═══$(RESET)\n"
	@printf "\n"
	@printf "$(ICON_SERVICE) $(CYAN)Checking running service and processes...$(RESET)\n"
	@if sudo systemctl is-active --quiet coolerdash.service; then \
		printf "  $(YELLOW)→$(RESET) Service running, stopping for update...\n"; \
		sudo systemctl stop coolerdash.service; \
		printf "  $(GREEN)→$(RESET) Service stopped\n"; \
	else \
		printf "  $(BLUE)→$(RESET) Service not running\n"; \
	fi
	@# Check for manual coolerdash processes and terminate them
	@COOLERDASH_COUNT=$$(pgrep -x coolerdash 2>/dev/null | wc -l); \
	if [ "$$COOLERDASH_COUNT" -gt 0 ]; then \
		printf "  $(YELLOW)→$(RESET) Found $$COOLERDASH_COUNT manual coolerdash process(es), terminating...\n"; \
		sudo killall -TERM coolerdash 2>/dev/null || true; \
		sleep 2; \
		REMAINING_COUNT=$$(pgrep -x coolerdash 2>/dev/null | wc -l); \
		if [ "$$REMAINING_COUNT" -gt 0 ]; then \
			printf "  $(RED)→$(RESET) Force killing $$REMAINING_COUNT remaining process(es)...\n"; \
			sudo killall -KILL coolerdash 2>/dev/null || true; \
		fi; \
		printf "  $(GREEN)→$(RESET) Manual processes terminated\n"; \
	else \
		printf "  $(BLUE)→$(RESET) No manual coolerdash processes found\n"; \
	fi
	@printf "\n"
	@printf "$(ICON_INFO) $(CYAN)Creating directories...$(RESET)\n"
	sudo mkdir -p /opt/coolerdash/bin
	sudo mkdir -p /opt/coolerdash/images
	sudo mkdir -p /var/cache/coolerdash
	@printf "$(ICON_SUCCESS) $(GREEN)Directories created$(RESET)\n"
	@printf "\n"
	@printf "$(ICON_INFO) $(CYAN)Copying files...$(RESET)\n"
	sudo cp $(BINDIR)/$(TARGET) /opt/coolerdash/bin/
	sudo chmod +x /opt/coolerdash/bin/$(TARGET)
	sudo cp images/face.png /opt/coolerdash/images/
	sudo cp $(README) /opt/coolerdash/
	@printf "  $(GREEN)→$(RESET) Program: /opt/coolerdash/bin/$(TARGET)\n"
	@printf "  $(GREEN)→$(RESET) Shutdown image: /opt/coolerdash/images/face.png\n"
	@printf "  $(GREEN)→$(RESET) Sensor image: will be created at runtime as coolerdash.png\n"
	@printf "  $(GREEN)→$(RESET) README: /opt/coolerdash/README.md\n"
	@printf "\n"
	@printf "$(ICON_SERVICE) $(CYAN)Installing service & documentation...$(RESET)\n"
	sudo cp $(SERVICE) /etc/systemd/system/
	sudo cp $(MANPAGE) /usr/share/man/man1/
	sudo mandb -q
	sudo systemctl daemon-reload
	@printf "  $(GREEN)→$(RESET) Service: /etc/systemd/system/coolerdash.service\n"
	@printf "  $(GREEN)→$(RESET) Manual: /usr/share/man/man1/coolerdash.1\n"
	@printf "\n"
	@printf "$(ICON_SERVICE) $(CYAN)Restarting service...$(RESET)\n"
	@if sudo systemctl is-enabled --quiet coolerdash.service; then \
		sudo systemctl start coolerdash.service; \
		printf "  $(GREEN)→$(RESET) Service started\n"; \
		printf "  $(GREEN)→$(RESET) Status: $$(sudo systemctl is-active coolerdash.service)\n"; \
	else \
		printf "  $(YELLOW)→$(RESET) Service not enabled\n"; \
		printf "  $(YELLOW)→$(RESET) Enable with: sudo systemctl enable coolerdash.service\n"; \
	fi
	@printf "\n"
	@printf "$(ICON_SUCCESS) $(WHITE)═══ INSTALLATION SUCCESSFUL ═══$(RESET)\n"
	@printf "\n"
	@printf "$(YELLOW)📋 Next steps:$(RESET)\n"
	@if sudo systemctl is-enabled --quiet coolerdash.service; then \
		printf "  $(GREEN)✓$(RESET) Service enabled and started\n"; \
		printf "  $(PURPLE)Check status:$(RESET)        sudo systemctl status coolerdash.service\n"; \
	else \
		printf "  $(PURPLE)Enable service:$(RESET)      sudo systemctl enable coolerdash.service\n"; \
		printf "  $(PURPLE)Start service:$(RESET)       sudo systemctl start coolerdash.service\n"; \
	fi
	@printf "  $(PURPLE)Show manual:$(RESET)         man coolerdash\n"
	@printf "\n"
	@printf "$(YELLOW)🔄 Available version:$(RESET)\n"
	@printf "  $(GREEN)Program:$(RESET) /opt/coolerdash/bin/coolerdash [mode]\n"
	@printf "\n"

# Uninstall Target
uninstall:
	@printf "\n"
	@printf "$(ICON_UNINSTALL) $(WHITE)═══ COOLERDASH UNINSTALLATION ═══$(RESET)\n"
	@printf "\n"
	@printf "$(ICON_WARNING) $(YELLOW)Stopping and disabling service...$(RESET)\n"
	sudo systemctl stop coolerdash.service 2>/dev/null || true
	sudo systemctl disable coolerdash.service 2>/dev/null || true
	@printf "$(ICON_SUCCESS) $(GREEN)Service stopped$(RESET)\n"
	@printf "\n"
	@printf "$(ICON_INFO) $(CYAN)Removing all files...$(RESET)\n"
	sudo rm -f /etc/systemd/system/coolerdash.service
	sudo rm -f /usr/share/man/man1/coolerdash.1
	sudo rm -f /opt/coolerdash/README.md
	sudo rm -f /opt/coolerdash/LICENSE
	sudo rm -f /opt/coolerdash/CHANGELOG.md
	sudo rm -f /opt/coolerdash/bin/$(TARGET)
	sudo rm -rf /opt/coolerdash/bin/
	sudo rm -rf /opt/coolerdash/images/
	sudo rm -rf /opt/coolerdash/
	sudo rm -rf /var/cache/coolerdash/
	sudo rm -f /usr/bin/coolerdash 2>/dev/null || true
	sudo rm -f /var/run/coolerdash.pid 2>/dev/null || true
	sudo rm -f /opt/coolerdash/coolerdash.png 2>/dev/null || true
	sudo rm -f /opt/coolerdash/images/coolerdash.png 2>/dev/null || true
	sudo rm -f /opt/coolerdash/images/*.png 2>/dev/null || true
	sudo rm -f /opt/coolerdash/*.md 2>/dev/null || true
	sudo rm -f /opt/coolerdash/*.txt 2>/dev/null || true
	sudo rm -f /opt/coolerdash/*.log 2>/dev/null || true
	sudo rm -rf /opt/coolerdash/* 2>/dev/null || true
	@printf "  $(RED)✗$(RESET) Service: /etc/systemd/system/coolerdash.service\n"
	@printf "  $(RED)✗$(RESET) Manual: /usr/share/man/man1/coolerdash.1\n"
	@printf "  $(RED)✗$(RESET) Program: /opt/coolerdash/bin/$(TARGET)\n"
	@printf "  $(RED)✗$(RESET) Documentation: /opt/coolerdash/README.md, LICENSE, CHANGELOG.md\n"
	@printf "  $(RED)✗$(RESET) Images: /opt/coolerdash/images/\n"
	@printf "  $(RED)✗$(RESET) Installation: /opt/coolerdash/\n"
	@printf "  $(RED)✗$(RESET) Cache: /var/cache/coolerdash/\n"
	@printf "  $(RED)✗$(RESET) Symlink: /usr/bin/coolerdash\n"
	@printf "\n"
	@printf "$(ICON_INFO) $(CYAN)Updating system...$(RESET)\n"
	sudo mandb -q
	sudo systemctl daemon-reload
	@printf "\n"
	@printf "$(ICON_SUCCESS) $(WHITE)═══ COMPLETE REMOVAL SUCCESSFUL ═══$(RESET)\n"
	@printf "\n"
	@printf "$(ICON_SUCCESS) $(GREEN)All coolerdash files have been completely removed$(RESET)\n"
	@printf "\n"

# Debug Build
debug: CFLAGS += -g -DDEBUG -fsanitize=address
debug: LIBS += -fsanitize=address
debug: $(TARGET)
	@printf "$(ICON_SUCCESS) $(GREEN)Debug build created with AddressSanitizer: $(BINDIR)/$(TARGET)$(RESET)\n"

# Service Management Targets
start:
	@printf "$(ICON_SERVICE) $(GREEN)Starting coolerdash service...$(RESET)\n"
	sudo systemctl start coolerdash.service
	@printf "$(ICON_SUCCESS) $(GREEN)Service started$(RESET)\n"

stop:
	@printf "$(ICON_SERVICE) $(YELLOW)Stopping coolerdash service...$(RESET)\n"
	sudo systemctl stop coolerdash.service
	@printf "$(ICON_SUCCESS) $(GREEN)Service stopped$(RESET)\n"

restart:
	@printf "$(ICON_SERVICE) $(CYAN)Restarting coolerdash service...$(RESET)\n"
	sudo systemctl restart coolerdash.service
	@printf "$(ICON_SUCCESS) $(GREEN)Service restarted$(RESET)\n"

status:
	@printf "$(ICON_INFO) $(CYAN)Service Status:$(RESET)\n"
	sudo systemctl status coolerdash.service

enable:
	@printf "$(ICON_SERVICE) $(GREEN)Enabling autostart...$(RESET)\n"
	sudo systemctl enable coolerdash.service
	@printf "$(ICON_SUCCESS) $(GREEN)Service will start automatically at boot$(RESET)\n"

disable:
	@printf "$(ICON_SERVICE) $(YELLOW)Disabling autostart...$(RESET)\n"
	sudo systemctl disable coolerdash.service
	@printf "$(ICON_SUCCESS) $(GREEN)Boot autostart disabled$(RESET)\n"

logs:
	@printf "$(ICON_INFO) $(CYAN)Live logs (Ctrl+C to exit):$(RESET)\n"
	sudo journalctl -u coolerdash.service -f

# Help
help:
	@printf "\n"
	@printf "$(WHITE)════════════════════════════════════════$(RESET)\n"
	@printf "$(WHITE)         COOLERDASH BUILD SYSTEM        $(RESET)\n"
	@printf "$(WHITE)════════════════════════════════════════$(RESET)\n"
	@printf "\n"
	@printf "$(YELLOW)🔨 Build Targets:$(RESET)\n"
	@printf "  $(GREEN)make$(RESET)          - Compiles the program\n"
	@printf "  $(GREEN)make clean$(RESET)    - Removes compiled files\n"
	@printf "  $(GREEN)make debug$(RESET)    - Debug build with AddressSanitizer\n"
	@printf "\n"
	@printf "$(YELLOW)📦 Installation:$(RESET)\n"
	@printf "  $(GREEN)make install$(RESET)  - Installs to /opt/coolerdash/bin/ (auto-installs dependencies)\n"
	@printf "  $(GREEN)make uninstall$(RESET)- Uninstalls the program\n"
	@printf "\n"
	@printf "$(YELLOW)⚙️  Service Management:$(RESET)\n"
	@printf "  $(GREEN)sudo systemctl start coolerdash.service$(RESET)    - Starts the service\n"
	@printf "  $(GREEN)sudo systemctl stop coolerdash.service$(RESET)     - Stops the service (sends face.png to LCD automatically)\n"
	@printf "  $(GREEN)sudo systemctl restart coolerdash.service$(RESET)  - Restarts the service\n"
	@printf "  $(GREEN)sudo systemctl status coolerdash.service$(RESET)   - Shows service status\n"
	@printf "  $(GREEN)sudo systemctl enable coolerdash.service$(RESET)   - Enables autostart\n"
	@printf "  $(GREEN)sudo systemctl disable coolerdash.service$(RESET)  - Disables autostart\n"
	@printf "  $(GREEN)sudo journalctl -u coolerdash.service -f$(RESET)   - Shows live logs\n"
	@printf "  $(BLUE)Note:$(RESET) Shortcuts available: make start/stop/restart/status/enable/disable/logs\n"
	@printf "  $(BLUE)Shutdown:$(RESET) Service automatically displays face.png when stopped (integrated in C code)\n"
	@printf "\n"
	@printf "$(YELLOW)📚 Documentation:$(RESET)\n"
	@printf "  $(GREEN)man coolerdash$(RESET) - Shows manual page\n"
	@printf "  $(GREEN)make help$(RESET)     - Shows this help\n"
	@printf "\n"
	@printf "$(YELLOW)🌍 README:$(RESET)\n"
	@printf "  $(GREEN)README.md$(RESET)         - 🇺🇸 English (main documentation)\n"
	@printf "\n"
	@printf "$(YELLOW)🔄 Version Usage:$(RESET)\n"
	@printf "  $(GREEN)Program:$(RESET) /opt/coolerdash/bin/coolerdash [mode]\n"
	@printf "\n"

.PHONY: clean install uninstall debug start stop restart status enable disable logs help detect-distro install-deps check-deps-for-install