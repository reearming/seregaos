#ifndef PAGE_H
#define PAGE_H
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)
#define PAGE_LARGE (1ULL << 7)

void vmm_map_page(uint64_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags);

#endif
