# Maintainer: DAMACHINE <christkue79@gmail.com>
pkgname=coolerdash
pkgver=1.25.07.13.1200
pkgrel=1
pkgdesc="CoolerDash - Modular C daemon for AIO LCD temperature monitoring via CoolerControl (BETA)"
arch=('x86_64')
url="https://github.com/damachine/coolerdash"
license=('MIT')
depends=('cairo' 'libcurl-gnutls' 'coolercontrol')
makedepends=('gcc' 'make' 'pkg-config')
optdepends=('nvidia-utils: for GPU temperature monitoring'
            'lm_sensors: for additional hardware monitoring')
backup=('opt/coolerdash/README.md')
install=coolerdash.install
source=()
sha256sums=()

prepare() {
    echo "================================================================"
    echo "⚠️  BETA SOFTWARE - EARLY DEVELOPMENT STAGE"
    echo "================================================================"
    echo "This software is in very early development and has been tested"
    echo "only on the developer's system with NZXT AIO Kraken 2023."
    echo ""
    echo "- Features may change or break without notice"
    echo "- Bugs and issues are expected"
    echo "- Use at your own risk"
    echo "- Please report issues to: https://github.com/damachine/coolerdash/issues"
    echo "================================================================"
    echo ""
    
    echo "================================================================"
    echo "Checking for existing manual installation..."
    echo "================================================================"
    
    # Check if manual installation exists
    local manual_installed=0
    local manual_service_exists=0
    local manual_binary_exists=0
    
    # Check for systemd service installed by Makefile
    if [[ -f "/etc/systemd/system/coolerdash.service" ]]; then
        echo "Found manual systemd service: /etc/systemd/system/coolerdash.service"
        manual_service_exists=1
        manual_installed=1
    fi
    
    # Check for binary installed by Makefile
    if [[ -f "/opt/coolerdash/bin/coolerdash" ]] && [[ ! -L "/usr/bin/coolerdash" ]]; then
        echo "Found manual binary installation: /opt/coolerdash/bin/coolerdash"
        manual_binary_exists=1
        manual_installed=1
    fi
    
    if [[ $manual_installed -eq 1 ]]; then
        echo ""
        echo "WARNING: Existing manual installation detected!"
        echo "This will conflict with the PKGBUILD installation."
        echo ""
        echo "Attempting automatic cleanup with 'make uninstall'..."
        echo ""
        
        # Try to run make uninstall if Makefile exists
        if [[ -f "Makefile" ]]; then
            # Stop service if running
            if systemctl is-active --quiet coolerdash.service 2>/dev/null; then
                echo "Stopping coolerdash service..."
                sudo systemctl stop coolerdash.service || true
            fi
            
            # Run make uninstall
            echo "Running 'sudo make uninstall'..."
            if sudo make uninstall; then
                echo "✅ Manual installation successfully removed"
            else
                echo "⚠️  Make uninstall failed, attempting manual cleanup..."
                
                # Manual cleanup if make uninstall fails
                sudo systemctl stop coolerdash.service 2>/dev/null || true
                sudo systemctl disable coolerdash.service 2>/dev/null || true
                sudo rm -f /etc/systemd/system/coolerdash.service
                sudo rm -f /usr/share/man/man1/coolerdash.1
                sudo rm -f /opt/coolerdash/bin/coolerdash
                sudo rm -f /opt/coolerdash/README.md
                sudo rm -rf /opt/coolerdash/images/
                sudo systemctl daemon-reload
                
                echo "✅ Manual cleanup completed"
            fi
        else
            echo "⚠️  No Makefile found, performing manual cleanup..."
            
            # Manual cleanup
            sudo systemctl stop coolerdash.service 2>/dev/null || true
            sudo systemctl disable coolerdash.service 2>/dev/null || true
            sudo rm -f /etc/systemd/system/coolerdash.service
            sudo rm -f /usr/share/man/man1/coolerdash.1
            sudo rm -f /opt/coolerdash/bin/coolerdash
            sudo rm -f /opt/coolerdash/README.md
            sudo rm -rf /opt/coolerdash/images/
            sudo systemctl daemon-reload
            
            echo "✅ Manual cleanup completed"
        fi
        
        echo ""
        echo "Proceeding with PKGBUILD installation..."
    else
        echo "✅ No conflicting manual installation found"
    fi
    
    echo "================================================================"
    echo "✅ NEW: AUTOMATIC DEVICE DETECTION - NO CONFIGURATION REQUIRED!"
    echo "================================================================"
    echo "This version automatically detects your AIO device UUID at runtime."
    echo "No manual UUID configuration is needed anymore!"
    echo ""
    echo "Prerequisites:"
    echo "1. Start CoolerControl: sudo systemctl start coolercontrold"
    echo "2. Ensure your AIO device is connected and detected by CoolerControl"
    echo "3. Run: makepkg -si"
    echo ""
    echo "The daemon will automatically find and use your AIO LCD device!"
    echo "================================================================"
    
    # Additional cleanup for any remaining conflicting files
    echo "Performing final cleanup of any conflicting files..."
    sudo rm -f /opt/coolerdash/images/coolerdash.png 2>/dev/null || true
    sudo rm -f /opt/coolerdash/images/face.png 2>/dev/null || true
    echo "✅ Final cleanup completed"
}

build() {
    # For local build: use current directory directly
    cd "$startdir"
    
    # Clean any previous builds
    make clean || true
    
    # Build with Arch Linux specific optimizations
    make CC=gcc CFLAGS="-Wall -Wextra -O2 -std=c99 -march=x86-64-v3 -Iinclude $(pkg-config --cflags cairo)" \
         LIBS="$(pkg-config --libs cairo) -lcurl -lm"
}

check() {
    cd "$startdir"
    
    # Basic functionality test
    if [[ -f "bin/coolerdash" ]]; then
        echo "Build successful - binary created"
        ./bin/coolerdash --help > /dev/null && echo "Help function works"
    else
        echo "ERROR: Binary not found"
        return 1
    fi
}

package() {
    cd "$startdir"
    
    # Create directory structure
    install -dm755 "$pkgdir/opt/coolerdash/bin"
    install -dm755 "$pkgdir/opt/coolerdash/images"
    install -dm755 "$pkgdir/usr/lib/systemd/system"
    install -dm755 "$pkgdir/usr/share/man/man1"
    install -dm755 "$pkgdir/usr/bin"
    install -dm755 "$pkgdir/var/cache/coolerdash"
    
    # Install binary
    install -Dm755 "bin/coolerdash" "$pkgdir/opt/coolerdash/bin/coolerdash"
    
    # Install default image
    if [[ -f "images/face.png" ]]; then
        install -Dm644 "images/face.png" "$pkgdir/opt/coolerdash/images/face.png"
    fi
    
    # Install coolerdash sensor image
    if [[ -f "images/coolerdash.png" ]]; then
        install -Dm644 "images/coolerdash.png" "$pkgdir/opt/coolerdash/images/coolerdash.png"
    fi
    
    # Install systemd service
    install -Dm644 "systemd/coolerdash.service" "$pkgdir/usr/lib/systemd/system/coolerdash.service"
    
    # Install man page
    install -Dm644 "man/coolerdash.1" "$pkgdir/usr/share/man/man1/coolerdash.1"
    
    # Install documentation
    install -Dm644 README.md "$pkgdir/opt/coolerdash/README.md"
    install -Dm644 LICENSE "$pkgdir/opt/coolerdash/LICENSE"
    install -Dm644 CHANGELOG.md "$pkgdir/opt/coolerdash/CHANGELOG.md"
    
    # Create symlink for system-wide access
    ln -sf "/opt/coolerdash/bin/coolerdash" "$pkgdir/usr/bin/coolerdash"
}
