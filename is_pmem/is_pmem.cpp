#include "pmem.h"
#include <asm/io.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <string>

const std::string PATH = "/dev/pmem0";

struct mem_range {
  phys_addr_t lower, upper;
};

mem_range get_pmem_range(const std::string &path) {
  fmode_t mode = FMODE_READ;
  struct block_device *device =
      blkdev_get_by_path(path.c_str, FMODE_READ, nullptr);
  struct pmem_device *pmem = bdev->bd_disk->private_data;
  phys_addr_t pmem_addr = pmem->phys_addr;
  size_t pmem_size = pmem->size;
  return {pmem_addr, pmem_addr + pmem_size};
}

bool is_pmem(void *addr) {
  phys_addr_t phys_addr = virt_to_phys(addr);
  mem_range pmem_range = get_pmem_range(PATH);
  return pmem_range.lower <= phys_addr && phys_addr < pmem_range.upper;
}