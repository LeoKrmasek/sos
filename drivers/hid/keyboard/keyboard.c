// This file contains general functions that are used as an interface
// to other parts of the kernel and to user space programs, and are used
// by all keyboard drivers (e.g. PS2 and others) the same

#include "keyboard.h"

static int keyboard_callbacks_pos = 0;
static void (*keyboard_callbacks[MAX_CALLBACK_COUNT])(struct keyboard_event);

// TODO: As long as we cannot allocate memory dynamically, use fixed one
static struct keyboard_event kbd_event;

static bool shift_active = false;

static bool keycode_is_printable[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,

    0 ,0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    0
};

static char keycode_to_char[256] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o' ,'p', 'a', 's', 'd', 'f',
    'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

    ',', '.', '/', '\\', '\'', ';', '-', '=', ' ', ' ', ' ', '\t', ' ',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ']', '[', ' ', ' ', ' ',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '*', '+', '.',

    ' '
};

static char keycode_to_char_shift[256] = {
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O' ,'P', 'A', 'S', 'D', 'F',
    'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

    '<', '>', '?', '|', '"', ':', '_', '+', ' ', ' ', ' ', '\t', ' ',

    ' ', ' ', ' ', ' ', ' ', ' ', ' ', '}', '{', ' ', ' ', ' ',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '*', '+', '.',

    ' '
};

void keyboard_add_callback(void (*callback)(struct keyboard_event)) {
    keyboard_callbacks[keyboard_callbacks_pos] = callback;
    keyboard_callbacks_pos++;
}

void keyboard_fire_event(enum keyboard_key kbdkey, bool released) {
    if (kbdkey == KEY_LSHIFT || kbdkey == KEY_RSHIFT) {
        shift_active = !released;
    }

    kbd_event.key = kbdkey;
    kbd_event.released = released;

    // TODO: We should make sure that they always match?
    if (kbdkey < 256) {
        kbd_event.printable = keycode_is_printable[kbdkey];

        if (shift_active) {
            kbd_event.ascii = keycode_to_char_shift[kbdkey];
        } else {
            kbd_event.ascii = keycode_to_char[kbdkey];
        }
    }

    for (int i = 0; i < keyboard_callbacks_pos; i++) {
        keyboard_callbacks[i](kbd_event);
    }
}
