#!/bin/bash
# create-release.sh - Creates a release tarball for GitHub
# This script is for the maintainer only, not for end users

VERSION="1.25.07.08.2234"
PKGNAME="aiolcdcam"
TARBALL="${PKGNAME}-${VERSION}.tar.gz"

echo "🚀 Creating release tarball for maintainer: $TARBALL"

# Create temporary directory structure
TMPDIR=$(mktemp -d)
RELEASE_DIR="$TMPDIR/$PKGNAME-$VERSION"

# Create the release directory
mkdir -p "$RELEASE_DIR"

echo "📂 Copying project files..."

# Copy all necessary files for release
cp -r src/ "$RELEASE_DIR/"
cp -r include/ "$RELEASE_DIR/"
cp -r systemd/ "$RELEASE_DIR/"
cp -r man/ "$RELEASE_DIR/"
cp -r images/ "$RELEASE_DIR/" 2>/dev/null || true  # Optional directory
cp Makefile "$RELEASE_DIR/"
cp README.md "$RELEASE_DIR/"
cp LICENSE "$RELEASE_DIR/"
cp CHANGELOG.md "$RELEASE_DIR/"

# Don't include development files in release tarball
# PKGBUILD and .install files are for AUR, not for source releases

echo "📦 Creating tarball..."

# Create tarball
cd "$TMPDIR"
tar -czf "$TARBALL" "$PKGNAME-$VERSION/"

# Move to current directory
mv "$TARBALL" "$OLDPWD/"

# Cleanup
rm -rf "$TMPDIR"

# Get back to original directory
cd "$OLDPWD"

echo "✅ Release tarball created: $TARBALL"
echo "📦 Size: $(du -h $TARBALL | cut -f1)"
echo "🔐 SHA256: $(sha256sum $TARBALL | cut -d' ' -f1)"

echo ""
echo "📋 Next steps for maintainer:"
echo "1. Upload $TARBALL to GitHub Releases as v$VERSION"
echo "2. Copy the SHA256 checksum above"
echo "3. Update PKGBUILD sha256sums with the checksum"
echo "4. Test PKGBUILD with: makepkg -si"
echo "5. Publish to AUR"
echo ""
echo "🔗 GitHub Release URL:"
echo "   https://github.com/damachine/aiolcdcam/releases/new"
