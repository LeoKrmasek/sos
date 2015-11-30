#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// TODO: Could probably be combined
struct page_dir_entry {
    uint16_t pt_addr_upper;
    uint8_t pt_addr_lower_avail_global;
    uint8_t flags;
} __attribute__((packed));

struct page_table_entry {
    uint16_t page_addr_upper;
    uint8_t page_addr_lower_avail_global;
    uint8_t flags;
} __attribute__((packed));

void paging_setup();

#endif PAGING_H
