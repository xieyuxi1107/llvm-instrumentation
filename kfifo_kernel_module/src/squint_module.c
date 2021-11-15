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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team");
MODULE_DESCRIPTION("A kernel module that opens");
MODULE_VERSION("0.01");

ssize_t SquintDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos);

// the number of elements in the fifo, this must be a power of 2
#define KFIFO_LEN 1024
#define KFIFO_THRESHOLD_LEN (KFIFO_LEN / 2)

#define PROC_NAME "squint"


// https://www.kernel.org/doc/htmldocs/kernel-api/API-DECLARE-KFIFO-PTR.html
static DECLARE_KFIFO_PTR(squint_dev, unsigned char);
spinlock_t lock;

// Declare interface to interact with virtual filesystem
struct file_operations SquintDeviceOps = {
	.owner = 	THIS_MODULE,
	.read = 	SquintDeviceRead,
	.llseek = 	noop_llseek
};

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

void write_flush_trace(void) {
    squint_fifo_write("flush\n");
}
EXPORT_SYMBOL(write_flush_trace);

void write_fence_trace(void) {
    squint_fifo_write("fence\n");
}
EXPORT_SYMBOL(write_fence_trace);

void write_store_trace(void) {
    squint_fifo_write("store\n");
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