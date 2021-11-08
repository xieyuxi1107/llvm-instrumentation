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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zachary Papanastasopoulos");
MODULE_DESCRIPTION("A kernel module that opens");
MODULE_VERSION("0.01");

ssize_t SquintDeviceRead(struct file *file, char __user *buf, size_t count, loff_t *ppos);

// the number of elements in the fifo, this must be a power of 2
#define KFIFO_LEN 1024
#define KFIFO_THRESHOLD_LEN (KFIFO_LEN / 2)

#define PROC_NAME "squint"

int asleep = 0;

// https://www.kernel.org/doc/htmldocs/kernel-api/API-DECLARE-KFIFO-PTR.html
static DECLARE_KFIFO_PTR(squint_dev, unsigned char);

/*typedef struct Metadata {
	unsigned int a;
	unsigned int b;
} Metadata;
*/
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
	printk(KERN_ERR "@ Squint: Starting lkm_example_init\n");

	// https://www.kernel.org/doc/htmldocs/kernel-api/API-kfifo-alloc.html
	ret = kfifo_alloc(&squint_dev, KFIFO_LEN, GFP_KERNEL);
	if (ret) {
		printk(KERN_ERR "@ Squint: ERROR kfifo_alloc\n");
		return ret;
	} else {
		printk(KERN_INFO "@ Squint: SUCCESS kfifo_alloc");
	}

	// remove_proc_entry(PROC_NAME, NULL);
	if (proc_create(PROC_NAME, 0, NULL, &SquintDeviceOps) == NULL) {
        printk(KERN_INFO "@ Squint: ERROR proc_create");
		kfifo_free(&squint_dev);
		return -ENOMEM;
	}

    kfifo_in(&squint_dev, "trace", 5);

	return 0;
}

static void __exit kfifo_squint_init(void) {
	printk(KERN_ERR "@ Squint: Starting lkm_example_exit\n");
	printk(KERN_INFO "@ Squint: kfifo_free");
	remove_proc_entry(PROC_NAME, NULL);
	kfifo_free(&squint_dev);
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
