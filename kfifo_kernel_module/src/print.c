
#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuxi Xie");
MODULE_DESCRIPTION("A kernel module that has multiple source file");
MODULE_VERSION("0.01");

extern void write_fence_trace(void);
extern void write_flush_trace(void);
extern void write_store_trace(unsigned long addr);

// void print_trace(void){
//   printk(KERN_ERR "xieyuxi: print_trace for ...");
// }

void print_trace(int instr){
  if(instr == 1){
    write_fence_trace();
  }
  if(instr == 2){
    write_flush_trace();
  }
}


void print_trace_addr(int instr, unsigned long addr){
  // TODO: check addr is within pm range
  write_store_trace(addr);
}
