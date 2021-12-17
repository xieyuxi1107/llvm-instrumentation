
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

void write_fence_trace_wrapper(){
  printf("xieyuxi: print_fence_trace\n");
  write_fence_trace();
}

void write_flush_trace_wrapper(unsigned long addr){
  printf("xieyuxi: print_flush_trace for %lu\n", addr);
  write_flush_trace();
}

void write_store_trace_wrapper(unsigned long addr, int sz){
  printf("xieyuxi: print_store_trace for %lu with size %d\n", addr, sz);
  write_store_trace(addr);
}

// void print_trace(int instr){
//   if(instr == 1){
//     write_fence_trace();
//   }
//   if(instr == 2){
//     write_flush_trace();
//   }
// }


// void print_trace_addr(int instr, unsigned long addr){
//   // TODO: check addr is within pm range
//   write_store_trace(addr);
// }
