// AegisOS Phase 0 — M0.3: Syscall tracer kernel module
// Uses kprobes to intercept execve and openat. This is the observability
// primitive that will eventually feed the AegisOS audit trail (M2.4).
// Logs to the kernel ring buffer (dmesg); production audit will write to
// a tamper-evident append-only log.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rajat (Krishnkant Singh)");
MODULE_DESCRIPTION("AegisOS Phase 0: syscall tracer via kprobes");
MODULE_VERSION("0.1");

// --- execve probe -----------------------------------------------------------

static int pre_execve(struct kprobe *p, struct pt_regs *regs)
{
    // First argument to execve is the filename pointer (userspace).
    // We copy it out carefully; silently skip if it faults.
    char filename[256];
    const char __user *ufilename = (const char __user *)regs->di;

    if (!ufilename)
        return 0;

    if (strncpy_from_user(filename, ufilename, sizeof(filename) - 1) > 0) {
        filename[sizeof(filename) - 1] = '\0';
        pr_info("aegis_trace: execve pid=%d comm=%s file=%s\n",
                current->pid, current->comm, filename);
    }
    return 0;
}

static struct kprobe kp_execve = {
    .symbol_name = "__x64_sys_execve",
    .pre_handler = pre_execve,
};

// --- openat probe -----------------------------------------------------------

static int pre_openat(struct kprobe *p, struct pt_regs *regs)
{
    char filename[256];
    // openat: rdi=dfd, rsi=filename, rdx=flags
    const char __user *ufilename = (const char __user *)regs->si;

    if (!ufilename)
        return 0;

    if (strncpy_from_user(filename, ufilename, sizeof(filename) - 1) > 0) {
        filename[sizeof(filename) - 1] = '\0';
        // Skip noisy kernel-internal and proc opens to keep output readable.
        if (filename[0] == '/' && strncmp(filename, "/proc", 5) != 0) {
            pr_info("aegis_trace: openat  pid=%d comm=%s file=%s\n",
                    current->pid, current->comm, filename);
        }
    }
    return 0;
}

static struct kprobe kp_openat = {
    .symbol_name = "__x64_sys_openat",
    .pre_handler = pre_openat,
};

// --- module init / exit -----------------------------------------------------

static int __init tracer_init(void)
{
    int ret;

    ret = register_kprobe(&kp_execve);
    if (ret < 0) {
        pr_err("aegis_trace: failed to register execve kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&kp_openat);
    if (ret < 0) {
        pr_err("aegis_trace: failed to register openat kprobe: %d\n", ret);
        unregister_kprobe(&kp_execve);
        return ret;
    }

    pr_info("aegis_trace: loaded — watching execve + openat\n");
    pr_info("aegis_trace: run 'dmesg | grep aegis_trace' to view events\n");
    return 0;
}

static void __exit tracer_exit(void)
{
    unregister_kprobe(&kp_execve);
    unregister_kprobe(&kp_openat);
    pr_info("aegis_trace: unloaded\n");
}

module_init(tracer_init);
module_exit(tracer_exit);
