#!/usr/bin/env bash
# Build all Phase 0 kernel modules.
# Run on a machine with kernel headers installed:
#   sudo apt install linux-headers-$(uname -r)   # Debian/Ubuntu
#   sudo pacman -S linux-headers                 # Arch

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODULES_DIR="${SCRIPT_DIR}/../kernel-modules"

modules=(char-device syscall-tracer lsm-stub)

for mod in "${modules[@]}"; do
    echo "=== Building ${mod} ==="
    make -C "${MODULES_DIR}/${mod}" all
    echo ""
done

echo "All modules built. Load order for testing:"
echo "  1. char_device.ko      (safe — character device)"
echo "  2. syscall_tracer.ko   (safe — kprobe logger)"
echo "  3. lsm_stub.ko         (development VM only — see source comments)"
