// AegisOS Phase 0 — M0.3: Linux Security Module stub
// This is not a production LSM. It hooks two operations (file_open and
// task_create) and logs them, demonstrating the LSM hook registration
// mechanism that the real AegisOS policy engine (Phase 2) will build on.
//
// To activate a real LSM it must be compiled into the kernel (not as a
// module) and enabled via CONFIG_LSM="aegis,...". This stub uses the
// security_add_hooks path available in kernels with CONFIG_SECURITY_STACKING
// or can be adapted to loadable LSM support (kernel 5.15+ with
// CONFIG_SECURITY_WRITABLE_HOOKS). For study purposes it compiles as a module
// and registers hooks when loaded.
//
// IMPORTANT: Load only on a development/VM kernel. Do not load on production.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/lsm_hooks.h>
#include <linux/security.h>
#include <linux/fs.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rajat (Krishnkant Singh)");
MODULE_DESCRIPTION("AegisOS Phase 0: LSM stub — hook skeleton for policy engine");
MODULE_VERSION("0.1");

// --- hook implementations ---------------------------------------------------

static int aegis_file_open(struct file *file)
{
    if (file && file->f_path.dentry) {
        pr_info("aegis_lsm: file_open pid=%d comm=%s inode=%lu\n",
                current->pid,
                current->comm,
                file->f_path.dentry->d_inode
                    ? file->f_path.dentry->d_inode->i_ino : 0UL);
    }
    return 0; // always allow in stub — real engine evaluates mode policy here
}

static int aegis_task_create(unsigned long clone_flags)
{
    pr_info("aegis_lsm: task_create pid=%d comm=%s flags=0x%lx\n",
            current->pid, current->comm, clone_flags);
    return 0; // always allow in stub
}

// --- hook table -------------------------------------------------------------

static struct security_hook_list aegis_hooks[] __lsm_ro_after_init = {
    LSM_HOOK_INIT(file_open,    aegis_file_open),
    LSM_HOOK_INIT(task_create,  aegis_task_create),
};

// --- module init / exit -----------------------------------------------------

static int __init aegis_lsm_init(void)
{
    // security_add_hooks is the correct API for stacked LSMs on modern kernels.
    // On kernels without stacking support this will fail to link — that is
    // expected; the stub is for study, not production loading.
    security_add_hooks(aegis_hooks, ARRAY_SIZE(aegis_hooks), "aegis");
    pr_info("aegis_lsm: stub loaded — file_open + task_create hooks active\n");
    pr_info("aegis_lsm: Phase 2 will replace allow-all with mode policy evaluation\n");
    return 0;
}

static void __exit aegis_lsm_exit(void)
{
    // LSM hooks intentionally cannot be removed at runtime — unloading an LSM
    // after hooks fire creates a use-after-free. This exit handler logs only.
    // In production AegisOS the LSM is compiled in, not a loadable module.
    pr_warn("aegis_lsm: unload requested — hooks remain registered until reboot\n");
}

module_init(aegis_lsm_init);
module_exit(aegis_lsm_exit);
