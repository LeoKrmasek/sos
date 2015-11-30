#include "arch/x86/idt.h"
#include "arch/x86/gdt.h"
#include "arch/x86/pic.h"
#include "drivers/hid/keyboard/keyboard.h"
#include "arch/x86/io.h"
#include "arch/x86/paging.h"

void print_keyboard(struct keyboard_event kbdevt) {
    char *vidmem = (char*) 0xb8000;

    if (kbdevt.printable && !kbdevt.released) {
        kprintc(kbdevt.ascii);
    }
}

void kernel_main()
{
    gdt_setup();
    pic_init(0x20, 0x28);
    idt_setup();
    paging_setup();

    keyboard_init();

    keyboard_add_callback(print_keyboard);

    kprint("Hello World");
    kprinti(12345);
    kprintx(0x12345678);

    while (1) {
    }
}
