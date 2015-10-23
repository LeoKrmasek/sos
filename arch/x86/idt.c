#include <stdint.h>
#include "idt.h"

void idt_set_entry(unsigned int pos, uint32_t base,
        uint16_t selector, uint8_t type_attributes)
{
    idt[pos].base_lower = base & 0xffff;
    idt[pos].selector = selector;
    idt[pos].unused = 0x0;
    idt[pos].type_attributes = type_attributes;
    idt[pos].base_higher = (base >> 16) & 0xffff;
}

extern void int_handler();
void idt_setup()
{
    idt_desc.limit = IDT_ENTRIES * sizeof(struct idt_entry) - 1;
    idt_desc.base = (uint32_t) (uintptr_t) idt;

    // TODO: set some entries
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, (uint32_t)(uintptr_t)&int_handler,
            0x8, 0x8e);
    }


    asm volatile("lidt (%0)" :: "m"(idt_desc));
    asm volatile("sti");
}
