#ifndef IO_H
#define IO_H

#include <stdint.h>

#define VIDMEM 0xb8000
#define VIDEO_SCREEN_WIDTH 80
#define VIDEO_SCREEN_HEIGHT 25

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline void io_wait() {
    asm volatile("outb %%al, $0x80" :: "a"(0));
}

void kprint(char *str);

void kprintc(char c);

void kprinti(int value);

void kprintx(int value);

#endif
