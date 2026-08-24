#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' not found."
    echo "Please run the install script first or specify the build folder."
    exit 1
fi

echo "==> Uninstalling application..."
if [ -w "$(ninja -C "$BUILD_DIR" -t targets | grep -m1 bin || echo /usr/local)" ]; then
    ninja -C "$BUILD_DIR" uninstall
else
    echo "Root permissions required to remove system files. Running with sudo:"
    sudo ninja -C "$BUILD_DIR" uninstall
fi

echo "==> Uninstallation complete!"