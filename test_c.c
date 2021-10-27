// #include <atomic>
#include <stdio.h>

int x, y;
// std::atomic<bool> ready{false};

void init()
{
  x = 2;
  y = 3;
  printf("Before fence1\n");
//   atomic_thread_fence(std::memory_order_release);
  printf("After fence1\n");
//   ready.store(true, std::memory_order_relaxed);
}

void flush_pm(){
  printf("flush_pm called \n");
}

void use()
{
//   if (ready.load(std::memory_order_relaxed))
  {
    printf("Before fence2\n");
    // atomic_thread_fence(std::memory_order_acquire);
    printf("After fence2\n");
    printf("%d \n", x + y);
  }
  flush_pm();
}

int main(int argc, char *argv[]){
  init();
  use();
  flush_pm();
  return 0;
}