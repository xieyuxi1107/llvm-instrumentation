/*
A lot of this code is derived from PMTest's kernel_module.c and kernel_module.h

*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/stacktrace.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team");
MODULE_DESCRIPTION("Kernel module that mocks");
MODULE_VERSION("0.01");

extern void write_flush_trace(void);

ssize_t TraceDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t NoTraceDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos);

#define PROC_NAME_TRACE "MockFStrace"
#define PROC_NAME_NO_TRACE "MockFSnotrace"

// #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
// #define HAVE_PROC_OPS
// #endif

// #ifdef HAVE_PROC_OPS
// static const struct proc_ops TraceDeviceOps = {
//   .proc_read = TraceDeviceRead,
//   .proc_lseek = noop_llseek
// };
// #else
static const struct file_operations TraceDeviceOps = {
  .owner = THIS_MODULE,
  .read = TraceDeviceRead,
  .llseek = noop_llseek
};
// #endif

// #ifdef HAVE_PROC_OPS
// static const struct proc_ops NoTraceDeviceOps = {
//   .proc_read = NoTraceDeviceRead,
//   .proc_lseek = noop_llseek
// };
// #else
static const struct file_operations NoTraceDeviceOps = {
  .owner = THIS_MODULE,
  .read = NoTraceDeviceRead,
  .llseek = noop_llseek
};
// #endif


ssize_t TraceDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // this could be any of the 3 public functions provided by the Squint module
    // because they all have comparable overhead
    write_flush_trace();
    return 0;
}

ssize_t NoTraceDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // do nothing
    return 0;
}

static int __init mockfs_init(void) {
	printk(KERN_ERR "@ Squint: Starting mockfs_init\n");


	if (proc_create(PROC_NAME_TRACE, 0, NULL, &TraceDeviceOps) == NULL) {
        printk(KERN_INFO "@ Squint: ERROR creating mockfs trace\n");
		return -ENOMEM;
	}

	if (proc_create(PROC_NAME_NO_TRACE, 0, NULL, &NoTraceDeviceOps) == NULL) {
        printk(KERN_INFO "@ Squint: ERROR creating mockfs no trace\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit mockfs_exit(void) {
	printk(KERN_ERR "@ Squint: Starting mockfs_exit\n");
	remove_proc_entry(PROC_NAME_TRACE, NULL);
	remove_proc_entry(PROC_NAME_NO_TRACE, NULL);
}

module_init(mockfs_init);
module_exit(mockfs_exit);
