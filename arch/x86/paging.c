#include <stdint.h>
#include <arch/x86/io.h>

#include "paging.h"

// TODO: This is just a test implementation, if we can
// replace the 4MB page dir entry with a link to a
// page table which then links to the real address
static struct page_table_entry test_page_table[1024];
static struct page_dir_entry test_dir_entry;

uint32_t page_dir_entry_to_int(struct page_dir_entry pd_entry) {
    return pd_entry.pt_addr_upper << 16
        | pd_entry.pt_addr_lower_avail_global << 8
        | pd_entry.flags;
}

void activate_page_table(struct page_dir_entry pd_entry) {
    asm volatile("xchg %bx, %bx");
    // reference to the first entry of the page dir (physical mem)
    uint32_t *mem = (uint32_t*)0xfffff000;
    //uint32_t *mem = (uint32_t*)0xfffff000;
    *mem = page_dir_entry_to_int(pd_entry);
}

void fill_page_dir_entry(uint32_t pt_addr, uint8_t avail_global,
        uint8_t flags, struct page_dir_entry *pd_entry)
{
    pd_entry->pt_addr_upper = pt_addr >> 4;
    pd_entry->pt_addr_lower_avail_global = (pt_addr & 0xff) << 4 | avail_global;
    pd_entry->flags = flags;
}


void fill_page_table_entry(uint32_t page_addr, uint8_t avail_global,
        uint8_t flags, struct page_table_entry *pt_entry)
{
    pt_entry->page_addr_upper = page_addr >> 4;
    pt_entry->page_addr_lower_avail_global = (page_addr & 0xff) << 4 | avail_global;
    pt_entry->flags = flags;
}

void paging_setup() {
    for (int i = 0; i < 1024; i++) {
        fill_page_table_entry(0x0 + i, 0x0, 0x3, &test_page_table[i]);
    }

    // Problem probably is, that the pointer to test_page_table points to a virtual address, not the physical
    // moreover, we gave the full address, not only the upper 20 bits + the array is not 4kb aligned
    fill_page_dir_entry((uintptr_t)test_page_table, 0x0, 0x3, &test_dir_entry);

    activate_page_table(test_dir_entry);

    for (int i = 0; i < 1024; i++) {
        asm volatile("invlpg (%0)" ::"r" (i * 0x1000) : "memory");
    }

    asm volatile("xchg %bx, %bx");
    uint32_t *mem = (uint32_t*)0xfffff000;
    kprintx(*mem);

    asm volatile("xchg %bx, %bx");

    char *mem2 = (char*)0xb8000;
    *mem2 = 'A';

    asm volatile("xchg %bx, %bx");
}
