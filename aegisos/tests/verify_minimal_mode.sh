#!/usr/bin/env bash
# AegisOS — Phase 2 M2.6: verify minimal mode enforcement
# Run this after loading the mode loader with the minimal.mode.toml descriptor.
# Each test prints PASS or FAIL. All tests must PASS before M2.6 is complete.

set -euo pipefail

PASS=0
FAIL=0

pass() { echo "PASS: $1"; ((PASS++)); }
fail() { echo "FAIL: $1"; ((FAIL++)); }

assert_denied() {
    local desc="$1"; shift
    if "$@" &>/dev/null; then
        fail "$desc (expected denial, got success)"
    else
        pass "$desc"
    fi
}

assert_allowed() {
    local desc="$1"; shift
    if "$@" &>/dev/null; then
        pass "$desc"
    else
        fail "$desc (expected success, got denial)"
    fi
}

echo "=== AegisOS minimal mode verification ==="
echo "Running as: $(id)"
echo ""

# Network: outbound should be blocked
assert_denied "outbound TCP blocked" \
    timeout 2 curl -s --connect-timeout 1 http://1.1.1.1

# Filesystem: /etc/shadow must not be readable
assert_denied "/etc/shadow is unreadable" \
    cat /etc/shadow

# Filesystem: /root must not be readable
assert_denied "/root is unreadable" \
    ls /root

# Filesystem: /usr/bin should be readable (in read_only_paths)
assert_allowed "/usr/bin readable" \
    ls /usr/bin/ls

# Resource: pids_max = 64; we should not be able to spawn 100 subshells
# (This test is illustrative — requires cgroup enforcement to be wired up)
echo "NOTE: pids_max enforcement requires cgroup loader (Phase 2 — skipping live test)"

# Audit: check that the audit log has an entry for this run
# (Requires the audit subsystem to be running)
if [ -f /var/log/aegis/audit.log ]; then
    if grep -q "minimal" /var/log/aegis/audit.log; then
        pass "audit log contains minimal mode entry"
    else
        fail "audit log exists but has no minimal mode entry"
    fi
else
    echo "SKIP: audit log not found (mode loader not running)"
fi

echo ""
echo "=== Results: ${PASS} passed, ${FAIL} failed ==="
[ "$FAIL" -eq 0 ]
