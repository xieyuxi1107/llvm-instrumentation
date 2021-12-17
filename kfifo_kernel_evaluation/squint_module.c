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
MODULE_DESCRIPTION("A kernel module that opens");
MODULE_VERSION("0.01");

ssize_t SquintDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos);

// the number of elements in the fifo, this must be a power of 2
// TODO: xieyuxi this may still be too small for holding all trace info
#define KFIFO_LEN 16384
#define KFIFO_THRESHOLD_LEN (KFIFO_LEN / 2)

#define PROC_NAME "squint"

// #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
// #define HAVE_PROC_OPS
// #endif


// https://www.kernel.org/doc/htmldocs/kernel-api/API-DECLARE-KFIFO-PTR.html
static DECLARE_KFIFO_PTR(squint_dev, unsigned char);
spinlock_t lock;

// Declare interface to interact with virtual filesystem
// struct file_operations SquintDeviceOps = {
// 	.owner = 	THIS_MODULE,
// 	.read = 	SquintDeviceRead,
// 	.llseek = 	noop_llseek
// };

// #ifdef HAVE_PROC_OPS
// static const struct proc_ops SquintDeviceOps = {
//   .proc_read = SquintDeviceRead,
//   .proc_lseek = noop_llseek
// };
// #else
static const struct file_operations SquintDeviceOps = {
  .owner = THIS_MODULE,
  .read = SquintDeviceRead,
  .llseek = noop_llseek
};
// #endif

// int HOW_MANY_ENTRIES_TO_STORE = 32; 

// the user interface to read metadata from kfifo
ssize_t SquintDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int ret;
    unsigned int copied;
	printk(KERN_INFO "@ Squint: start reading\n");
    ret = kfifo_to_user(&squint_dev, buf, count, &copied);
	printk(KERN_INFO "@ Squint: end reading\n");

    return ret ? ret : copied;

}

static int __init kfifo_squint_init(void) {
    int ret;
	printk(KERN_ERR "@ Squint: Starting kfifo_squint_init\n");

    spin_lock_init(&lock);

	// https://www.kernel.org/doc/htmldocs/kernel-api/API-kfifo-alloc.html
	ret = kfifo_alloc(&squint_dev, KFIFO_LEN, GFP_KERNEL);
	if (ret) {
		printk(KERN_ERR "@ Squint: ERROR kfifo_alloc\n");
		return ret;
	} else {
		printk(KERN_INFO "@ Squint: SUCCESS kfifo_alloc\n");
	}

	// remove_proc_entry(PROC_NAME, NULL);
	if (proc_create(PROC_NAME, 0, NULL, &SquintDeviceOps) == NULL) {
        printk(KERN_INFO "@ Squint: ERROR proc_create\n");
		kfifo_free(&squint_dev);
		return -ENOMEM;
	}

	return 0;
}

void squint_fifo_write(char *input) {
    kfifo_in_spinlocked(&squint_dev, input, strlen(input), &lock);
}

void print_trace(void){
	unsigned long stack_entries[32];
	unsigned int len = stack_trace_save(&stack_entries[0], 32, 0);
	int i = 0;
	for (; i < len; i++) {
		char buf[200];
		snprintf(buf, 200, "[<%px>] %pS\n", (void *) stack_entries[i], (void *) stack_entries[i]);
		squint_fifo_write(buf);
	}
}

void write_flush_trace(void) {
    squint_fifo_write("@ flush\n");
	print_trace();
}
EXPORT_SYMBOL(write_flush_trace);

void write_fence_trace(void) {
    squint_fifo_write("@ fence\n");
	print_trace();
}
EXPORT_SYMBOL(write_fence_trace);

void write_store_trace(unsigned long addr) {
	char buf[40];
	snprintf(buf, 40, "@ store: %lu\n", addr);
    squint_fifo_write(buf);
	print_trace();
}
EXPORT_SYMBOL(write_store_trace);

static void __exit kfifo_squint_exit(void) {
	printk(KERN_ERR "@ Squint: Starting kfifo_squint_exit\n");
	printk(KERN_INFO "@ Squint: kfifo_free\n");
	remove_proc_entry(PROC_NAME, NULL);
	kfifo_free(&squint_dev);
}

module_init(kfifo_squint_init);
module_exit(kfifo_squint_exit);
