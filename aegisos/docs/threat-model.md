# AegisOS Threat Model — Phase 0 Draft

> **Status:** Draft — M0.5  
> **Author:** Rajat (Krishnkant Singh)  
> **Last revised:** 2026-05-12

---

## 1. System Description

AegisOS is a Linux-based adaptive security platform. Its central innovation is a
policy engine that enforces discrete operational modes (e.g., student, developer,
security-research) at runtime via LSM policies, Linux namespaces, and cgroups.

**In-scope components (Phase 0–3):**
- Hardened Linux kernel + custom LSM
- Mode descriptor files (YAML/TOML)
- Mode loader daemon
- Audit trail subsystem
- CLI tooling

**Out-of-scope until Phase 4+:**
- GUI / display stack
- Android port
- AI integration
- Offensive tooling distribution

---

## 2. Assets

| Asset | Sensitivity | Notes |
|---|---|---|
| Mode descriptor files | High | Defines privilege boundaries — tampering breaks isolation |
| Audit log | High | Integrity violation defeats accountability |
| LSM policy binary | Critical | Compromise = arbitrary privilege escalation |
| Mode loader daemon | Critical | Root-level service; loader compromise = full system compromise |
| User data (per-mode) | High | Cross-mode data leakage is the primary failure case |
| Kernel | Critical | Baseline for all security guarantees |
| Boot chain | Critical | Pre-OS compromise undermines everything above |

---

## 3. Adversaries

### 3.1 Local unprivileged user
- **Capability:** Code execution within their current mode
- **Goal:** Escalate to a higher-privilege mode or read data from another mode
- **Likelihood:** High — this is the primary threat
- **Assumed knowledge:** Knows the mode system exists; can read public AegisOS docs

### 3.2 Compromised application
- **Capability:** Running as a normal user within a mode, possibly with a
  known CVE in a sandboxed tool
- **Goal:** Escape the mode sandbox; pivot to host namespace
- **Likelihood:** Medium

### 3.3 Malicious mode descriptor
- **Capability:** Crafted `.mode` file introduced via package manager or
  social engineering
- **Goal:** Install a permissive policy disguised as a restrictive one
- **Likelihood:** Low (Phase 3+); relevant once mode-sharing exists

### 3.4 Physical attacker (cold boot / evil maid)
- **Capability:** Physical access to powered-off or locked machine
- **Goal:** Extract secrets, install persistent firmware/boot implant
- **Likelihood:** Low for Phase 0–3; addressed in Phase 4 (measured boot + FDE)

### 3.5 Kernel exploit
- **Capability:** Exploiting a kernel CVE from within a sandboxed process
- **Goal:** Full ring-0 compromise
- **Likelihood:** Low (mitigated by custom kernel config + hardening)
- **Note:** This threat is accepted at Phase 0–2; addressed in Phase 4

---

## 4. Attack Surfaces

| Surface | Exposure | Phase introduced |
|---|---|---|
| Syscall interface | All user processes | Phase 0 (traced in M0.3) |
| Mode loader IPC/socket | Any process requesting mode switch | Phase 2 |
| Mode descriptor parser | Whoever writes descriptors | Phase 2 |
| Audit subsystem | Processes that generate events | Phase 2 |
| Package manager | All installed software | Phase 1 |
| Network stack (per-mode) | Mode-specific exposure | Phase 3 |
| Boot loader / UEFI | Pre-OS window | Phase 4 |

---

## 5. Key Assumptions

1. The kernel is trusted until proven otherwise. Kernel exploits are out of
   scope until Phase 4.
2. The root user is trusted. AegisOS does not protect against a malicious admin.
3. Hardware is trusted (no supply-chain firmware attacks) until Phase 4.
4. Mode descriptors are authored by the system administrator, not end users.
5. The LSM is the primary enforcement point — if it fails, the mode boundary fails.

---

## 6. Threats (STRIDE)

### Spoofing
- **T-S1:** A process spoofs its mode identity to the loader.  
  *Mitigation:* Mode tokens bound to a PID namespace; loader verifies via kernel interface, not userspace claim.

### Tampering
- **T-T1:** An attacker modifies a mode descriptor file.  
  *Mitigation:* Descriptors signed; loader rejects unsigned or modified files (Phase 2).
- **T-T2:** Audit log is tampered with.  
  *Mitigation:* Write-once append log; cryptographic chain (Phase 2).

### Repudiation
- **T-R1:** A mode transition is denied but no record exists.  
  *Mitigation:* Immutable audit trail (M2.4); every transition logged before execution.

### Information Disclosure
- **T-I1:** Process in research mode reads files belonging to student mode.  
  *Mitigation:* Separate user namespaces + per-mode filesystem mounts (Phase 3).
- **T-I2:** A sandboxed tool exfiltrates data over the network.  
  *Mitigation:* Per-mode network namespace with firewall rules (Phase 3).

### Denial of Service
- **T-D1:** A process in one mode exhausts CPU/memory, starving other modes.  
  *Mitigation:* cgroup resource limits enforced by mode loader (Phase 2).

### Elevation of Privilege
- **T-E1:** A process escapes its mode sandbox via namespace breakout.  
  *Mitigation:* Seccomp filter + LSM deny rules for dangerous capabilities.
- **T-E2:** The mode loader itself is exploited to grant arbitrary privileges.  
  *Mitigation:* Loader runs with minimal capability set; privilege separated (Phase 2 — M2.5).

---

## 7. Accepted Risks (Phase 0–2)

| Risk | Reason accepted | Addressed in |
|---|---|---|
| Kernel CVE exploitation | Mitigation requires custom patching beyond current scope | Phase 4 |
| Firmware / UEFI implant | Measured boot not implemented yet | Phase 4 |
| Mode descriptor forgery | Signing infrastructure not built yet | Phase 2 |

---

## 8. Open Questions

- [ ] Which LSM? SELinux (mature, complex) vs AppArmor (simpler, less expressive). Decision by M2.1.
- [ ] Mode-switch authentication method: passphrase, hardware token, or boot-time only? Decision by M3.4.
- [ ] Single audit log for all modes or per-mode logs with a merge step?
- [ ] How does the loader verify mode descriptor integrity before the signing infrastructure exists?

---

## 9. Revision History

| Date | Change | Author |
|---|---|---|
| 2026-05-12 | Initial draft (M0.5) | Rajat |
