# Maintainer: DAMACHINE <christkue79@gmail.com>
pkgname=aiolcdcam
pkgver=1.25.07.08.2234
pkgrel=1
pkgdesc="LCD AIO CAM - Modular C daemon for NZXT Kraken LCD temperature monitoring via CoolerControl (BETA)"
arch=('x86_64')
url="https://github.com/damachine/aiolcdcam"
license=('MIT')
depends=('cairo' 'libcurl-gnutls' 'coolercontrol')
makedepends=('gcc' 'make' 'pkg-config')
optdepends=('nvidia-utils: for GPU temperature monitoring'
            'lm_sensors: for additional hardware monitoring')
backup=('opt/aiolcdcam/README.md')
install=aiolcdcam.install
source=("$pkgname-$pkgver.tar.gz::https://github.com/damachine/$pkgname/releases/download/v$pkgver/$pkgname-$pkgver.tar.gz")
sha256sums=('0019dfc4b32d63c1392aa264aed2253c1e0c2fb09216f8e2cc269bbfb8bb49b5')

prepare() {
    cd "$pkgname-$pkgver"
    
    echo "================================================================"
    echo "⚠️  BETA SOFTWARE - EARLY DEVELOPMENT STAGE"
    echo "================================================================"
    echo "This software is in very early development and has been tested"
    echo "only on the developer's system with NZXT Kraken 2023."
    echo ""
    echo "- Features may change or break without notice"
    echo "- Bugs and issues are expected"
    echo "- Use at your own risk"
    echo "- Please report issues to: https://github.com/damachine/aiolcdcam/issues"
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
    if [[ -f "/etc/systemd/system/aiolcdcam.service" ]]; then
        echo "Found manual systemd service: /etc/systemd/system/aiolcdcam.service"
        manual_service_exists=1
        manual_installed=1
    fi
    
    # Check for binary installed by Makefile
    if [[ -f "/opt/aiolcdcam/bin/aiolcdcam" ]] && [[ ! -L "/usr/bin/aiolcdcam" ]]; then
        echo "Found manual binary installation: /opt/aiolcdcam/bin/aiolcdcam"
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
            if systemctl is-active --quiet aiolcdcam.service 2>/dev/null; then
                echo "Stopping aiolcdcam service..."
                sudo systemctl stop aiolcdcam.service || true
            fi
            
            # Run make uninstall
            echo "Running 'sudo make uninstall'..."
            if sudo make uninstall; then
                echo "✅ Manual installation successfully removed"
            else
                echo "⚠️  Make uninstall failed, attempting manual cleanup..."
                
                # Manual cleanup if make uninstall fails
                sudo systemctl stop aiolcdcam.service 2>/dev/null || true
                sudo systemctl disable aiolcdcam.service 2>/dev/null || true
                sudo rm -f /etc/systemd/system/aiolcdcam.service
                sudo rm -f /usr/share/man/man1/aiolcdcam.1
                sudo rm -f /opt/aiolcdcam/bin/aiolcdcam
                sudo rm -f /opt/aiolcdcam/README.md
                sudo systemctl daemon-reload
                
                echo "✅ Manual cleanup completed"
            fi
        else
            echo "⚠️  No Makefile found, performing manual cleanup..."
            
            # Manual cleanup
            sudo systemctl stop aiolcdcam.service 2>/dev/null || true
            sudo systemctl disable aiolcdcam.service 2>/dev/null || true
            sudo rm -f /etc/systemd/system/aiolcdcam.service
            sudo rm -f /usr/share/man/man1/aiolcdcam.1
            sudo rm -f /opt/aiolcdcam/bin/aiolcdcam
            sudo rm -f /opt/aiolcdcam/README.md
            sudo systemctl daemon-reload
            
            echo "✅ Manual cleanup completed"
        fi
        
        echo ""
        echo "Proceeding with PKGBUILD installation..."
    else
        echo "✅ No conflicting manual installation found"
    fi
    
    echo "================================================================"
    echo "IMPORTANT: UUID CONFIGURATION REQUIRED BEFORE BUILD!"
    echo "================================================================"
    echo "You MUST configure your device UUID in include/config.h BEFORE"
    echo "running makepkg, otherwise the daemon will not work!"
    echo ""
    echo "Steps to configure UUID:"
    echo "1. Start CoolerControl: sudo systemctl start coolercontrold"
    echo "2. Find your UUID: curl http://localhost:11987/devices | jq"
    echo "3. Edit include/config.h and set AIO_UUID to your device UUID"
    echo "4. THEN run: makepkg -si"
    echo ""
    echo "Post-installation reconfiguration is also possible at:"
    echo "/opt/aiolcdcam/include/config.h (rebuild with: cd /opt/aiolcdcam && sudo make install)"
    echo "================================================================"
}

build() {
    cd "$pkgname-$pkgver"
    
    # Build with Arch Linux specific optimizations
    make CC=gcc CFLAGS="-Wall -Wextra -O2 -std=c99 -march=x86-64-v3 -Iinclude $(pkg-config --cflags cairo)" \
         LIBS="$(pkg-config --libs cairo) -lcurl -lm"
}

check() {
    cd "$pkgname-$pkgver"
    
    # Basic functionality test
    if [[ -f "bin/aiolcdcam" ]]; then
        echo "Build successful - binary created"
        ./bin/aiolcdcam --help > /dev/null && echo "Help function works"
    else
        echo "ERROR: Binary not found"
        return 1
    fi
}

package() {
    cd "$pkgname-$pkgver"
    
    # Create directory structure
    install -dm755 "$pkgdir/opt/aiolcdcam/bin"
    install -dm755 "$pkgdir/opt/aiolcdcam/image"
    install -dm755 "$pkgdir/opt/aiolcdcam/include"
    install -dm755 "$pkgdir/opt/aiolcdcam/src"
    install -dm755 "$pkgdir/usr/lib/systemd/system"
    install -dm755 "$pkgdir/usr/share/man/man1"
    install -dm755 "$pkgdir/usr/bin"
    
    # Install binary
    install -Dm755 "bin/aiolcdcam" "$pkgdir/opt/aiolcdcam/bin/aiolcdcam"
    
    # Install source code for UUID configuration
    install -Dm644 include/*.h "$pkgdir/opt/aiolcdcam/include/"
    install -Dm644 src/*.c "$pkgdir/opt/aiolcdcam/src/"
    install -Dm644 Makefile "$pkgdir/opt/aiolcdcam/"
    
    # Install default image
    if [[ -f "images/face.png" ]]; then
        install -Dm644 "images/face.png" "$pkgdir/opt/aiolcdcam/images/face.png"
    fi
    
    # Install systemd service
    install -Dm644 "systemd/aiolcdcam.service" "$pkgdir/usr/lib/systemd/system/aiolcdcam.service"
    
    # Install man page
    install -Dm644 "man/aiolcdcam.1" "$pkgdir/usr/share/man/man1/aiolcdcam.1"
    
    # Install documentation
    install -Dm644 README.md "$pkgdir/opt/aiolcdcam/README.md"
    install -Dm644 LICENSE "$pkgdir/opt/aiolcdcam/LICENSE"
    install -Dm644 CHANGELOG.md "$pkgdir/opt/aiolcdcam/CHANGELOG.md"
    
    # Create symlink for system-wide access
    ln -sf "/opt/aiolcdcam/bin/aiolcdcam" "$pkgdir/usr/bin/aiolcdcam"
}
