#include <atomic>
#include <stdio.h>

int x, y;
std::atomic<bool> ready{false};

// void init()
// {
//   x = 2;
//   y = 3;
//   printf("Before fence1\n");
//   atomic_thread_fence(std::memory_order_release);
//   printf("After fence1\n");
//   ready.store(true, std::memory_order_relaxed);
// }

void nvdimm_flush(int& x){
  printf("flush_pm called \n");
}

void volatile_flush(int& x){
  printf("flush_pm called \n");
}

void use()
{
  x = 2;
  volatile_flush(x);
  if (x==1)
  {
    atomic_thread_fence(std::memory_order_acquire);
  }else{
    nvdimm_flush(x);
  }
}

int main(int argc, char *argv[]){
  // init();
  use();
  // flush_pm(y);
  return 0;
}