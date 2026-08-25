#!/usr/bin/env bash
set -euo pipefail

# meson.build needs to turn on install: true if this were to work.

REPO_URL="https://github.com/realmaacke/saver.git"
TEMP_DIR=$(mktemp -d)

cleanup() {
    rm -rf "$TEMP_DIR"
}
trap cleanup EXIT

echo "==> Fetching saver..."
git clone --depth 1 "$REPO_URL" "$TEMP_DIR/saver"
cd "$TEMP_DIR/saver"

echo "==> Configuring and building..."
meson setup build --prefix=/usr/local --buildtype=release
ninja -C build

echo "==> Installing..."
if [ "$EUID" -ne 0 ]; then
    sudo ninja -C build install
else
    ninja -C build install
fi

echo "==> Installed successfully! Run 'saver' to get started."