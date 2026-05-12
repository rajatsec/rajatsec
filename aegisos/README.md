# AegisOS

> A Linux-based adaptive security platform with policy-driven operational modes.

AegisOS enforces discrete operational modes (student, developer, security-research)
at runtime via a custom Linux Security Module, Linux namespaces, and cgroups.
The policy engine is the product — everything else serves it.

---

## Current status: Phase 0 — Foundations

| Milestone | Status |
|---|---|
| M0.1 Linux kernel study (Love + Bovet) | In progress |
| M0.2 Linux From Scratch build | In progress |
| M0.3 Three kernel modules | **Code committed** — build & test below |
| M0.4 C fluency + Rust tool | **Rust stub committed** |
| M0.5 Written threat model | **Draft committed** |

---

## Repository layout

```
aegisos/
├── docs/
│   ├── threat-model.md          # M0.5 — assets, adversaries, STRIDE analysis
│   └── decisions/
│       └── M1.1-foundation-choice.md  # Fill in by month 7
├── kernel-modules/
│   ├── char-device/             # M0.3a — /dev/aegis_char character device
│   ├── syscall-tracer/          # M0.3b — kprobe-based execve/openat tracer
│   └── lsm-stub/                # M0.3c — LSM hook skeleton (dev VM only)
├── tools/
│   └── sysinfo-rs/              # M0.4 — Rust tool reading /proc
├── policy/
│   ├── schemas/
│   │   └── mode-descriptor.schema.json   # Phase 2 spec
│   └── modes/
│       └── minimal.mode.toml             # Phase 2 M2.6 first mode
├── tests/
│   └── verify_minimal_mode.sh            # Phase 2 enforcement test
└── scripts/
    ├── build-modules.sh
    └── build-rust-tool.sh
```

---

## Building

### Prerequisites

```bash
# Debian/Ubuntu
sudo apt install build-essential linux-headers-$(uname -r) rustup

# Arch
sudo pacman -S base-devel linux-headers rustup
```

### Kernel modules

```bash
./scripts/build-modules.sh
```

Load individually on a development VM:

```bash
# Character device (safe on any machine)
cd kernel-modules/char-device && make load
echo "hello" | sudo tee /dev/aegis_char && sudo cat /dev/aegis_char

# Syscall tracer (safe on any machine)
cd kernel-modules/syscall-tracer && make load
dmesg | grep aegis_trace

# LSM stub — DEVELOPMENT VM ONLY, see source comments
cd kernel-modules/lsm-stub && make load
```

### Rust sysinfo tool

```bash
./scripts/build-rust-tool.sh
sudo ./tools/sysinfo-rs/target/release/aegis-sysinfo
```

---

## Phase 0 exit criterion

> You can explain, without notes, how a Linux machine boots from firmware to login prompt.

Checklist before moving to Phase 1:
- [ ] Read *Linux Kernel Development* (Love) cover to cover; notes written
- [ ] LFS build completed and booted
- [ ] All three kernel modules build, load, and produce correct output
- [ ] Rust sysinfo tool compiles and runs cleanly
- [ ] Threat model reviewed and signed off by yourself

---

## Roadmap

See [ROADMAP.md](../ROADMAP.md) for the full multi-year plan.

> **Guiding principle:** The policy engine is the product. No GUI, no Android
> port, no AI integration, no offensive tooling until Phase 4.

---

## License

GPL-2.0 — kernel modules must be GPL; rest of the project follows.
