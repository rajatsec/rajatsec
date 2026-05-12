#!/usr/bin/env bash
# Build the Phase 0 Rust sysinfo tool.
# Requires: rustup (https://rustup.rs) with stable toolchain

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOL_DIR="${SCRIPT_DIR}/../tools/sysinfo-rs"

echo "=== Building aegis-sysinfo (Rust) ==="
cargo build --release --manifest-path "${TOOL_DIR}/Cargo.toml"

echo ""
echo "Binary: ${TOOL_DIR}/target/release/aegis-sysinfo"
echo "Run:    sudo ${TOOL_DIR}/target/release/aegis-sysinfo"
