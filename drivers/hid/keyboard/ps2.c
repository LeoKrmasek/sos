#include <stdbool.h>
#include <stdint.h>
#include <arch/x86/io.h>
#include "keyboard.h"

// Mapping table from scancode set 2 standard keys (not multimedia)
// to the key codes
static enum keyboard_key s2_keycodes[256] = {
    KEY_UNUSED, KEY_F9, KEY_UNUSED, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12,
    KEY_UNUSED, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, KEY_BACKTICK, KEY_UNUSED,
    KEY_UNUSED, KEY_ALT, KEY_LSHIFT, KEY_UNUSED, KEY_LCTRL, KEY_Q, KEY_1, KEY_UNUSED,
    KEY_UNUSED, KEY_UNUSED, KEY_Z, KEY_S, KEY_A, KEY_W, KEY_2, KEY_UNUSED,
    KEY_UNUSED, KEY_C, KEY_X, KEY_D, KEY_E, KEY_4, KEY_3, KEY_UNUSED,
    KEY_UNUSED, KEY_SPACE, KEY_V, KEY_F, KEY_T, KEY_R, KEY_5, KEY_UNUSED,
    KEY_UNUSED, KEY_N, KEY_B, KEY_H, KEY_G, KEY_Y, KEY_6, KEY_UNUSED,
    KEY_UNUSED, KEY_UNUSED, KEY_M, KEY_J, KEY_U, KEY_7, KEY_8, KEY_UNUSED,
    KEY_UNUSED, KEY_COMMA, KEY_K, KEY_I, KEY_O, KEY_0, KEY_9, KEY_UNUSED,
    KEY_UNUSED, KEY_DOT, KEY_SLASH, KEY_L, KEY_SEMICOLON, KEY_P, KEY_MINUS, KEY_UNUSED,
    KEY_UNUSED, KEY_UNUSED, KEY_APOSTROPHE, KEY_UNUSED, KEY_LSQ_BRACKET, KEY_EQUALS, KEY_UNUSED, KEY_UNUSED,
    KEY_CAPSLOCK, KEY_RSHIFT, KEY_ENTER, KEY_RSQ_BRACKET, KEY_UNUSED, KEY_BACKSLASH, KEY_UNUSED, KEY_UNUSED,
    KEY_UNUSED, KEY_UNUSED, KEY_BACKSPACE, KEY_UNUSED, KEY_UNUSED, KEY_1_KP, KEY_UNUSED, KEY_4_KP,
    KEY_7_KP, KEY_UNUSED, KEY_UNUSED, KEY_UNUSED, KEY_0_KP, KEY_DOT_KP, KEY_2_KP, KEY_5_KP,
    KEY_6_KP, KEY_8_KP, KEY_ESC, KEY_NUMLOCK, KEY_F11, KEY_PLUS_KP, KEY_3_KP, KEY_MINUS_KP,
    KEY_TIMES_KP, KEY_9_KP, KEY_SCROLLLOCK, KEY_UNUSED, KEY_UNUSED, KEY_UNUSED, KEY_F7
};

// All possible states for our keycode finite state machine
static void fsm_start(uint8_t);
static void fsm_release(uint8_t);
static void fsm_multimedia(uint8_t);
static void fsm_multimedia_release(uint8_t);
static void fsm_print_1(uint8_t);
static void fsm_print_2(uint8_t);
static void kbd_wait_outbuf();
static void set_scancode(uint8_t);


// FSM: Keep track of the current state of the state machine
static void (*current_state)(uint8_t) = fsm_start;

// The state machine start states for each scancode set
// TODO: Currently scancode set 1 and 3 are missing and will trigger
// a fault interrupt
static void (*scancode_state_starts[3])(uint8_t) = { 0x0, fsm_start, 0x0 };

void keyboard_init() {
    set_scancode(2);
}

void keyboard_irq_handler() {
    uint8_t scancode;

    kbd_wait_outbuf();
    scancode = inb(0x60);
    current_state(scancode);
}

// Methods for waiting for keyboard
static void kbd_wait_outbuf() {
    // TODO: Can we add some idle loops or is kbd controller so fast?
    uint8_t status;
    do {
        status = inb(0x64);
    } while ((status & 0x1) == 0);
}

static void kbd_wait_inbuf() {
    // TODO: Can we add some idle loops or is kbd controller so fast?
    uint8_t status;
    do {
        status = inb(0x64);
    } while ((status & 0x2) != 0);
}

static void set_scancode(uint8_t scancode_nr) {
    if (scancode_nr < 1 || scancode_nr > 3)
        return;

    outb(0x60, 0xf0);
    kbd_wait_inbuf();
    outb(0x60, scancode_nr);
    kbd_wait_inbuf();
    
    // disable translation
    outb(0x64, 0x20);
    kbd_wait_outbuf();
    uint8_t status = inb(0x60);

    kbd_wait_inbuf();
    outb(0x64, 0x60);
    kbd_wait_inbuf();
    outb(0x60, status & 0x3f);

    current_state = scancode_state_starts[scancode_nr - 1];
}

// FSM function implementation
// These functions read scancodes and jump to correct next function
static void fsm_start(uint8_t byte) {
    if (byte == 0xe0) {
        current_state = fsm_multimedia;
    } else if (byte == 0xf0) {
        current_state = fsm_release;
    } else {
        if (s2_keycodes[byte] != KEY_UNUSED) {
            keyboard_fire_event(s2_keycodes[byte], false);
        }
    }
}

static void fsm_multimedia(uint8_t byte) {
    if (byte == 0xf0) {
        current_state = fsm_multimedia_release;
    } else if (byte == 0x12) {
        current_state = fsm_print_1;
    } else {
        // TODO: multimedia key byte pressed
        current_state = fsm_start;
    }
}

static void fsm_release(uint8_t byte) {
    if (s2_keycodes[byte] != KEY_UNUSED) {
        keyboard_fire_event(s2_keycodes[byte], true);
    }

    current_state = fsm_start;
}

static void fsm_multimedia_release(uint8_t byte) {
            char *vidmem = (char*) 0xb8000;
            vidmem[0] = 'x';
    // TODO: Multimedia key byte released
        current_state = fsm_start;
}

static void fsm_print_1(uint8_t byte) {
    if (byte == 0xe0) {
        current_state = fsm_print_2;
    } else {
        // TODO: Failure
    }
}

static void fsm_print_2(uint8_t byte) {
    if (byte == 0x7c) {
        // TODO key = PRINT pressed
        current_state = fsm_start;
    } else {
        // TODO: Failure
    }
}
