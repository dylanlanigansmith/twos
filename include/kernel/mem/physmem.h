#include <kernel/kernel.h>

#define PHYS_USED 1
#define PHYS_FREE 0

size_t physmem_init(void* mb_mmap, size_t mmap_size);



phys_addr_t phys_malloc(size_t size);

void phys_free(phys_addr_t addr, size_t size);