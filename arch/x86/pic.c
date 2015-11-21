#include <arch/x86/io.h>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

#define ICW1 0x11 // ICW1 initialize and ICW4 is needed, cascading
                  // 8 byte interrupts vectors
#define ICW4 0x01 // x86 architecture, normal EOI, not buffered, sequential

void pic_init(int pic1_pos, int pic2_pos) {
    unsigned char a1, a2;
     
    // save masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, ICW1);
    outb(PIC2_CMD, ICW1);

    outb(PIC1_DATA, pic1_pos);
    outb(PIC2_DATA, pic2_pos);

    outb(PIC1_DATA, 4); // Master PIC: There is a slave at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2); // Slave PIC: cascade identity 0000 0010

    outb(PIC1_DATA, ICW4);
    outb(PIC2_DATA, ICW4);

    // restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}
