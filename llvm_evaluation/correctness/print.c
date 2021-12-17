#include <stdio.h>

// void print_trace(int instr){
//   printf("xieyuxi: print_trace for %d\n", instr);
//   // errs() << F->getName() << " ";
//   // errs() << *I << "\n";
// }
void write_fence_trace_wrapper(){
  printf("xieyuxi: print_fence_trace\n");
}

void write_flush_trace_wrapper(unsigned long addr){
  printf("xieyuxi: print_flush_trace for %lu", addr);
}

void write_store_trace_wrapper(unsigned long addr, int sz){
  printf("xieyuxi: print_store_trace for %lu with size %d\n", addr, sz);
}

// void print_trace_addr(int instr, unsigned long addr){
//   // TODO: check addr is within pm range
//   printf("xieyuxi: print_trace for %d\n", instr);
//   if(instr == 0)
//     printf("xieyuxi: store addr is %lu\n", addr);
  
// }