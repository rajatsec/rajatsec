// AegisOS Phase 0 — M0.3: Character device kernel module
// Registers /dev/aegis_char. Demonstrates module lifecycle, file_operations,
// and the read/write path — building blocks for the mode loader's kernel interface.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>

#define DEVICE_NAME "aegis_char"
#define CLASS_NAME  "aegis"
#define BUF_SIZE    256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rajat (Krishnkant Singh)");
MODULE_DESCRIPTION("AegisOS Phase 0: character device stub");
MODULE_VERSION("0.1");

static int    major;
static char   message[BUF_SIZE];
static size_t message_len;

static struct class  *aegis_class;
static struct device *aegis_device;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};

static int __init aegis_char_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("aegis_char: failed to register major number\n");
        return major;
    }

    aegis_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(aegis_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(aegis_class);
    }

    aegis_device = device_create(aegis_class, NULL, MKDEV(major, 0),
                                 NULL, DEVICE_NAME);
    if (IS_ERR(aegis_device)) {
        class_destroy(aegis_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(aegis_device);
    }

    pr_info("aegis_char: registered at major %d -> /dev/%s\n",
            major, DEVICE_NAME);
    return 0;
}

static void __exit aegis_char_exit(void)
{
    device_destroy(aegis_class, MKDEV(major, 0));
    class_unregister(aegis_class);
    class_destroy(aegis_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("aegis_char: unregistered\n");
}

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("aegis_char: device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("aegis_char: device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    size_t to_copy;

    if (*offset >= message_len)
        return 0;

    to_copy = min(len, message_len - (size_t)*offset);

    if (copy_to_user(buf, message + *offset, to_copy))
        return -EFAULT;

    *offset += to_copy;
    pr_info("aegis_char: sent %zu bytes to user\n", to_copy);
    return to_copy;
}

static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *offset)
{
    size_t to_copy = min(len, (size_t)(BUF_SIZE - 1));

    if (copy_from_user(message, buf, to_copy))
        return -EFAULT;

    message[to_copy] = '\0';
    message_len = to_copy;
    pr_info("aegis_char: received %zu bytes: \"%s\"\n", to_copy, message);
    return to_copy;
}

module_init(aegis_char_init);
module_exit(aegis_char_exit);
