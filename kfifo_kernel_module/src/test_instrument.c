#include <linux/init.h>
#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include </home/xieyuxi/shared/kernel_module/print.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuxi Xie");
MODULE_DESCRIPTION("A kernel module that has multiple source file");
MODULE_VERSION("0.01");
int x, y;


void init(void)
{
  x = 2;
  y = 3;
  // printf("Before fence1\n");
  // atomic_thread_fence(std::memory_order_release);
  // printf("After fence1\n");
  // ready.store(true, std::memory_order_relaxed);
}

void flush_pm(void){
  printk(KERN_ERR "flush_pm called \n");
}

static void use(void)
{
  // if (ready.load(std::memory_order_relaxed))
  // {
    // printf("Before fence2\n");
    // atomic_thread_fence(std::memory_order_acquire);
    // printf("After fence2\n");
    printk(KERN_ERR "%d \n", x + y);
  // }
  flush_pm();
}

static int __init test_fence_init(void) {
	printk(KERN_ERR "@ Testing: Starting test_fence_init\n");
//  print_trace(555);
  init();
  use();
  flush_pm();
	return 0;
}

static void __exit test_fence_exit(void) {
	printk(KERN_ERR "@ Testing: Starting test_fence_exit\n");
}

module_init(test_fence_init);
module_exit(test_fence_exit);
