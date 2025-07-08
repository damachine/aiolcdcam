# Maintainer: DAMACHINE <christkue79@gmail.com>
pkgname=aiolcdcam
pkgver=0.1.3
pkgrel=1
pkgdesc="LCD AIO CAM - Modular C daemon for NZXT Kraken LCD temperature monitoring via CoolerControl"
arch=('x86_64')
url="https://github.com/damachine/aiolcdcam"
license=('MIT')
depends=('cairo' 'libcurl-gnutls' 'coolercontrol')
makedepends=('gcc' 'make' 'pkg-config')
optdepends=('nvidia-utils: for GPU temperature monitoring'
            'lm_sensors: for additional hardware monitoring')
backup=('opt/aiolcdcam/README.md')
install=aiolcdcam.install
source=("$pkgname-$pkgver.tar.gz::https://github.com/damachine/$pkgname/archive/v$pkgver.tar.gz")
sha256sums=('SKIP')  # Replace with actual checksum when releasing

prepare() {
    cd "$pkgname-$pkgver"
    
    # Inform user about UUID configuration requirement
    echo "================================================================"
    echo "IMPORTANT: You MUST configure your device UUID before first use!"
    echo "================================================================"
    echo "1. Start CoolerControl: sudo systemctl start coolercontrold"
    echo "2. Find your UUID: curl http://localhost:11987/devices | jq"
    echo "3. After installation, edit: /opt/aiolcdcam/include/config.h"
    echo "4. Set KRAKEN_UID to your device's UUID"
    echo "5. Rebuild: cd /opt/aiolcdcam && sudo make install"
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
    if [[ -f "image/face.png" ]]; then
        install -Dm644 "image/face.png" "$pkgdir/opt/aiolcdcam/image/face.png"
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
