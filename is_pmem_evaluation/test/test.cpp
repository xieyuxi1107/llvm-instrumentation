#include <string>
#include <utility>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

// class mapper: takes a filename and memory maps it
class mapper
{
  std::string fname;
  char *addr_start;
  size_t addr_len;

public:
  mapper(const std::string &filename) : fname{filename} {}
  std::pair<char *, char *> map()
  {
    int fd = open(fname.c_str(), O_RDWR);
    if (fd < 0)
      exit(1);
    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if (err < 0)
      exit(2);
    char *addr_start = (char *)mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr_start == MAP_FAILED)
      exit(3);
    addr_len = statbuf.st_size;
    close(fd);
    return std::make_pair(addr_start, addr_start + addr_len);
  }
  ~mapper()
  {
    munmap((void *)addr_start, addr_len);
  }
};

// main: memory maps a file, prints out which address is pmem, which isn't
int main()
{
  std::string fname = "/mnt/pmem/testfile";
  mapper map{fname};
  auto [start, end] = map.map();
  *start = '\'';
  std::cout << (void *)start << " is persistent memory\n";
  std::cout << (void *)&fname << " is not persistent memory\n";
}