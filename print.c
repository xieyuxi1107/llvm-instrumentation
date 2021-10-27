#include <stdio.h>

void print_trace(int instr){
  printf("xieyuxi: print_trace for %d\n", instr);
  // errs() << F->getName() << " ";
  // errs() << *I << "\n";
}


void print_trace_addr(int instr, unsigned long addr){
  // TODO: check addr is within pm range
  printf("xieyuxi: print_trace for %d\n", instr);
  if(instr == 0)
    printf("xieyuxi: store addr is %lu\n", addr);
  
}