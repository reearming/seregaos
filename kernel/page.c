#include "fbuffer.h"
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)
#define PAGE_LARGE (1ULL << 7)

#define MAX_TABLES 64
__attribute__((aligned(4096))) static uint64_t page_table_pool[MAX_TABLES][512];
static size_t allocated_tables = 0;

static uint64_t *allocate_table_page() {
    if (allocated_tables >= MAX_TABLES) {
        printf("%RSTATIC TABLE POOL OVERFLOW! %d >= %d", allocated_tables, MAX_TABLES);
        for (;;) asm("cli; hlt");
    }
    uint64_t *new_table = page_table_pool[allocated_tables++];

    for (int i = 0; i < 512; i++) {
        new_table[i] = 0;
    }
    return new_table;
}

void vmm_map_page(uint64_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    virt &= ~0xFFFULL;
    phys &= ~0xFFFULL;

    size_t pml4_idx = (virt >> 39) & 0x1FF;
    size_t pdpt_idx = (virt >> 30) & 0x1FF;
    size_t pd_idx = (virt >> 21) & 0x1FF;
    size_t pt_idx = (virt >> 12) & 0x1FF;

    uint64_t *pdpt;
    if (pml4[pml4_idx] & PAGE_PRESENT) {
        pdpt = (uint64_t *)(pml4[pml4_idx]);
    }
    else {
    print("ERROR\n");
        pdpt = allocate_table_page();
        pml4[pml4_idx] = ((uint64_t)pdpt) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t *pd;
    if (pdpt[pdpt_idx] & PAGE_PRESENT) {
        pd = (uint64_t *)(pdpt[pdpt_idx] & ~0xFFFULL);
    }
    else {
        pd = allocate_table_page();
        pdpt[pdpt_idx] = ((uint64_t)pd) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t *pt;
    if (pd[pd_idx] & PAGE_PRESENT) {
        pt = (uint64_t *)(pd[pd_idx] & ~0xFFFULL);
    }
    else {
        pt = allocate_table_page();
        pd[pd_idx] = ((uint64_t)pt) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    pt[pt_idx] = phys | flags | PAGE_PRESENT;

    asm("invlpg (%0)" : : "r"(virt) : "memory");
}

/*void init_hardware_mappings() {
    uint64_t cr3_val;
    asm("mov %%cr3, %0" : "=r"(cr3_val));
    uint64_t *pml4 (uint64_t *)(cr3_val & ~0xFFFULL);

    uint64_t ioapic
}*/
