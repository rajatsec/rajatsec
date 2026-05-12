# AegisOS — Multi-Year Roadmap

> **North star (one sentence):** A Linux-based adaptive security platform with policy-driven operational modes.

---

## Guiding Principles

These are non-negotiable. They exist because solo OS projects almost always die from the same cause: premature complexity.

1. **Foundations before features.** Skip a phase and the project collapses.
2. **One mode working end-to-end beats five modes half-built.**
3. **The policy engine is the product.** Everything else serves it.
4. **No GUI, no Android port, no AI integration, no offensive tooling until Phase 4.** Period.
5. **Every phase ships something testable.** No long, invisible stretches.
6. **Write before you code.** Threat models, trade-off docs, and specs come before implementations.

---

## Phase 0 — Foundations (Months 1–6)

**Goal:** Earn the right to start building.

**Milestones:**
- **M0.1** Read and exercise *Linux Kernel Development* (Love) and selected chapters of *Understanding the Linux Kernel*.
- **M0.2** Build Linux From Scratch (LFS) end-to-end, at least once.
- **M0.3** Write three small kernel modules: a character device, a syscall tracer, and a stub Linux Security Module (LSM).
- **M0.4** Reach fluency in C; reach functional competence in Rust (ship one Rust systems tool, however small).
- **M0.5** Produce a written threat model for AegisOS — assets, adversaries, attack surfaces, assumptions.

**Exit criterion:** You can explain, without notes, how a Linux machine boots from firmware to login prompt.

**Do not yet:** pick a logo, write a manifesto, plan the GUI, register a domain.

---

## Phase 1 — Base Distribution (Months 7–15)

**Goal:** A minimal, reproducible, CLI-only AegisOS that boots reliably on target hardware.

**Milestones:**
- **M1.1** Foundation decision (deadline: month 7). Choose Debian-based, Arch-based, or Yocto/Buildroot. Write a trade-off document defending the choice. Do not reverse it without writing a new one.
- **M1.2** Reproducible build pipeline: same input produces the same ISO, every time.
- **M1.3** Custom hardened kernel config. Strip everything unused.
- **M1.4** Define target hardware (e.g., 2 GB RAM, dual-core) and a boot-time budget. Hit it.
- **M1.5** Init system decision (systemd / runit / s6). Trade-off document required.
- **M1.6** First internal alpha ISO. Boots, logs in, networks.

**Exit criterion:** A stranger could flash your ISO to a USB stick and boot it on the target hardware.

**Do not yet:** any GUI, any "mode" code, any pentesting tools.

---

## Phase 2 — Policy Framework (Months 16–24) — *The Core*

**Goal:** Build the actual innovation. Everything before this was preparation.

**Milestones:**
- **M2.1** Mastery of SELinux *or* AppArmor (pick one). Write three non-trivial policies for real services.
- **M2.2** Design the **mode descriptor** — a declarative file (YAML or TOML) that defines a mode's permissions, network rules, exposed tools, and filesystem access. Spec it on paper before coding it.
- **M2.3** Build the **mode loader** — a service that reads a descriptor and enforces it via SELinux/AppArmor, Linux namespaces, and cgroups.
- **M2.4** Immutable audit trail for every mode transition.
- **M2.5** Threat-model the policy engine itself. What if the loader is compromised? What if a mode escapes?
- **M2.6** Author the first mode descriptor — trivial, e.g., a "minimal" mode with almost no privileges. Prove the loader enforces it with a test script.

**Exit criterion:** You can switch between two trivial modes and demonstrate the privilege difference with a reproducible script.

**Do not yet:** more than two modes, any GUI mode switcher, any user-facing polish.

---

## Phase 3 — Operational Modes (Months 25–36)

**Goal:** Build the modes that prove the concept.

**Milestones:**
- **M3.1** *Student mode* — locked down, no developer toolchain, restricted network, fully audited.
- **M3.2** *Developer mode* — full toolchain, broader filesystem access, separate user namespace.
- **M3.3** *Security research mode* — sandboxed offensive tools, isolated network namespace, no host filesystem access by default.
- **M3.4** Mode transition security: define and enforce *who* can switch and *how* (passphrase, hardware key, boot-time only — your call, justified in writing).
- **M3.5** Cross-mode isolation testing. Actively try to leak data from research mode to student mode. Document attempts and defenses.
- **M3.6** First public beta. At least two modes, working.

**Exit criterion:** Someone other than you uses AegisOS in two modes for a full week and writes feedback you can act on.

**Do not yet:** scale to many modes, build a "mode marketplace," or add anything cute.

---

## Phase 4 — Hardening & Optimization (Months 37–48)

**Goal:** Make the "secure" claim defensible.

**Milestones:**
- **M4.1** Secure boot chain: measured boot, verified kernel, signed packages.
- **M4.2** Full-disk encryption integrated with mode unlocking.
- **M4.3** Low-resource benchmarks: measured boot time, RAM usage, idle CPU on the target hardware. Publish the numbers.
- **M4.4** External security review — even informal. Invite one or two people who know systems security to try to break it. Document findings and fixes.
- **M4.5** Reproducible builds verified by a second party.
- **M4.6** Begin GUI work: minimal Wayland-based display stack with a permanently visible mode indicator.

**Exit criterion:** You can list ten ways someone tried to break your isolation, and explain how each was handled.

---

## Phase 5 — Polish & Public Release (Months 49+)

**Goal:** Present the work well enough to be respected.

**Milestones:**
- **M5.1** Documentation site: architecture, threat model, install guide, mode-authoring guide.
- **M5.2** Demo video showing mode switching with privilege differences visualized.
- **M5.3** Public 1.0 release. Freeze scope at the start of this phase.
- **M5.4** Submit a talk or paper to a regional Linux or security conference.
- **M5.5** A GitHub repo with real stars, real issues, and real contributors — earned, not solicited.

---

## Decision Gates

Before moving to the next phase, answer in writing:

1. Did I hit every milestone of the current phase?
2. What did I learn that should change the next phase?
3. What new risks emerged?

If you can't answer all three, repeat the phase. Do not move on.

---

## Risk Register

| Risk | Mitigation |
|---|---|
| Scope creep into GUI / Android / AI | This roadmap. Refuse Phase 4 GUI work until Phase 3 exit criterion is met. |
| Solo burnout | Ship something visible every two months, even small. |
| Foundation regret (Debian → Arch six months in) | Write the M1.1 trade-off document carefully. Do not reverse without a new one. |
| Policy engine fragility | M2.5 threat model. Treat the loader as adversarial code. |
| "Just one more feature" before 1.0 | Declare 1.0 scope at start of Phase 5 and freeze it. |
| Picking too many modes too early | Cap at two modes through M2.6. Three is the Phase 3 limit. |

---

## What "Done" Looks Like

At the end of this roadmap you will have:

- A working hardened Linux distribution with at least three distinct operational modes.
- A documented policy framework that someone else could extend.
- Measurable security and resource benchmarks.
- A portfolio that proves systems-engineering competence to any employer.
- Optionally: a research paper, conference talk, or open-source community.
