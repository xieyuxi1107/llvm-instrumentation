/*
Module to test dynamic linking of squint kfifo.
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

extern void write_fence_trace(void);
extern void write_flush_trace(void);
extern void write_store_trace(void);

static int __init test_kfifo_squint_init(void) {
	printk(KERN_ERR "@ Squint: Starting test_kfifo_squint_init\n");
    write_fence_trace();
    write_flush_trace();
    write_store_trace();
	return 0;
}

static void __exit test_kfifo_squint_exit(void) {
	printk(KERN_ERR "@ Squint: Starting test_kfifo_squint_exit\n");
}

module_init(test_kfifo_squint_init);
module_exit(test_kfifo_squint_exit);
