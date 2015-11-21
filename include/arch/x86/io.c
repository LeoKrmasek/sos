#include "io.h"
#include "math.h"

static uint16_t curpos = 0;

static void shift_screen_up() {
    char *vidmem = (char*)0xb8000;

    // Move existing lines one row up
    for (int line = 1; line < VIDEO_SCREEN_HEIGHT; line++) {
        for (int line_byte = 0; line_byte < 2 * VIDEO_SCREEN_WIDTH; line_byte++) {
            int offset = 2 * VIDEO_SCREEN_WIDTH * line + line_byte;

            vidmem[offset - 2 * VIDEO_SCREEN_WIDTH] = vidmem[offset];
        }
    }
    // Reset text on "new" line
    for (int col = 0; col < VIDEO_SCREEN_WIDTH; col++) {
        int offset = 2 * VIDEO_SCREEN_WIDTH * (VIDEO_SCREEN_HEIGHT - 1) + 2 * col;

        vidmem[offset] = ' ';
    }
    // Reset colors on "new" line
    for (int col = 0; col < VIDEO_SCREEN_WIDTH; col++) {
        int offset = 2 * VIDEO_SCREEN_WIDTH * (VIDEO_SCREEN_HEIGHT - 1) + 2 * col + 1;

        vidmem[offset] = 0xf;
    }

    curpos -= VIDEO_SCREEN_WIDTH;
}

static void increment_cursor() {
    curpos++;

    if (curpos >= VIDEO_SCREEN_WIDTH * VIDEO_SCREEN_HEIGHT) {
        shift_screen_up();
    }
}

void kprint(char *str) {
    for (int i = 0; str[i] != 0; i++) {
        kprintc(str[i]);
    }
}

void kprintc(char c) {
    char *vidmem = (char*)VIDMEM;
    vidmem[2*curpos] = c;
    vidmem[2*curpos + 1] = 0x9;

    increment_cursor();
}

static short no_of_digits(int value) {
    short len = 0;
    int tmp = 1;

    while (tmp < value) {
        len++;
        tmp = (tmp << 3) + (tmp << 1);
    }

    return len;
}

void kprinti(int value) {
    short digits = no_of_digits(value);

    int divisor = (int)power(10, digits - 1);
    for (int i = divisor; i > 0; i /= 10) {
        short digit = value / i;
        value %= i;
        kprintc((char)(digit + 48));
    }
}
