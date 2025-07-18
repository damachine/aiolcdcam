# Project coding standards and packaging notes (see README for details)
#
# - All code must be C99 compliant (ISO/IEC 9899:1999)
# - Versioning: custom scheme "0.year.month.day.hourminute" (see VERSION file)
# - Always update both pkgver (here) and VERSION before every commit/release
# - All code comments, documentation, and commit messages must be in English
# - All user questions are answered in German (see .github/copilot-instructions.md)
# - All dependencies must be clearly listed and documented here and in README
# - Build flags must enforce C99 and recommended optimizations
# - See README and AUR-README for further details
#
# Example version: 0.2025.07.16.1234 (Year 2025, July 16th, 12:34)
#
# Maintainer: DAMACHINE <christkue79@gmail.com>
#
# --- Dependency notes ---
# - 'cairo', 'libcurl-gnutls', 'coolercontrol' are required for core functionality
# - 'nvidia-utils' and 'lm_sensors' are optional for extended hardware monitoring
# - 'ttf-roboto' is required for proper font rendering on the LCD
# - All dependencies are documented in README and AUR-README
# ------------------------

# NOTE: Before every commit, update pkgver to match the current release version.
# Versioning follows: 0.year.month.day.hourminute (e.g., 0.2025.07.16.1234)
# Always ensure VERSION in Makefile matches pkgver here.
#
# Build flags are set for C99 compliance and x86-64-v3 optimizations.
# All dependencies must be documented and handled in the build system.
#
# All comments in this file must be in English (see coding standards).

pkgname=coolerdash-git
pkgver() {
    cd "$srcdir/coolerdash"
    # Version: 0.<year>.<month>.<day>.<commits> (z.B. 0.2025.07.18.123)
    echo "0.$(date +%Y.%m.%d).$(git rev-list --count HEAD)"
}
pkgrel=1
pkgdesc="CoolerDash - build from latest git (AIO LCD system monitor for CoolerControl)"
arch=('x86_64')
url="https://github.com/damachine/coolerdash"
license=('MIT')
depends=('cairo' 'libcurl-gnutls' 'coolercontrol')
makedepends=('git' 'gcc' 'make' 'pkg-config' 'ttf-roboto')
optdepends=('nvidia-utils: for GPU temperature monitoring'
            'lm_sensors: for additional hardware monitoring')
backup=('opt/coolerdash/README.md')
install=coolerdash.install
source=("git+https://github.com/damachine/coolerdash.git")
sha256sums=('SKIP')

prepare() {
    cd "$srcdir/coolerdash"
    # Optional: Clean up or patch if needed
}

build() {
    cd "$srcdir/coolerdash"
    make clean || true
    make
}

check() {
    cd "$srcdir/coolerdash"
    if [[ -f "bin/coolerdash" ]]; then
        echo "Build successful - binary created"
        ./bin/coolerdash --help > /dev/null && echo "Help function works"
    else
        echo "ERROR: Binary not found"
        return 1
    fi
}

package() {
    cd "$srcdir/coolerdash"
    install -dm755 "$pkgdir/opt/coolerdash/bin"
    install -dm755 "$pkgdir/opt/coolerdash/images"
    install -dm755 "$pkgdir/usr/lib/systemd/system"
    install -dm755 "$pkgdir/usr/share/man/man1"
    install -dm755 "$pkgdir/usr/bin"
    install -dm755 "$pkgdir/var/cache/coolerdash"
    install -Dm755 "bin/coolerdash" "$pkgdir/opt/coolerdash/bin/coolerdash"
    if [[ -f "images/face.png" ]]; then
        install -Dm644 "images/face.png" "$pkgdir/opt/coolerdash/images/face.png"
    fi
    if [[ -f "images/coolerdash.png" ]]; then
        install -Dm644 "images/coolerdash.png" "$pkgdir/opt/coolerdash/images/coolerdash.png"
    fi
    install -Dm644 "systemd/coolerdash.service" "$pkgdir/usr/lib/systemd/system/coolerdash.service"
    install -Dm644 "man/coolerdash.1" "$pkgdir/usr/share/man/man1/coolerdash.1"
    install -Dm644 README.md "$pkgdir/opt/coolerdash/README.md"
    install -Dm644 LICENSE "$pkgdir/opt/coolerdash/LICENSE"
    install -Dm644 CHANGELOG.md "$pkgdir/opt/coolerdash/CHANGELOG.md"
    ln -sf "/opt/coolerdash/bin/coolerdash" "$pkgdir/usr/bin/coolerdash"
}
