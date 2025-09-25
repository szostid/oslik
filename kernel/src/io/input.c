#include <ports.h>
#include <stdint.h>
#include <stdio.h>

#define KEYBOARD_DATA_PORT 0x60

// clang-format off

static const char *scancode_set[83] = {
    "ERR",
    "ESC",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "=",
    "bs",
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "[",
    "]",
    "Enter",
    "CTRL",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    ";",
    "'",
    "`",
    "LShift",
    "\\",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    ",",
    ".",
    "/",
    "RShift",
    "PrtSc",
    "Alt",
    "Space",
    "Caps",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "Num",
    "Scroll",
    "Home",
    "Up",
    "PgUp",
    "-",
    "Left",
    "Center",
    "Right",
    "+",
    "End",
    "Down",
    "PgDn",
    "Ins",
    "Del",
};

// clang-format on

void on_key_press()
{
    uint8_t scancode = inb(0x60);
    uint8_t actual_key = scancode & (~128);

    if ((scancode & 128) == 0)
    {
        if (actual_key > 83)
        {
            printf("Key #%d pressed\n", actual_key);
        }
        else
        {
            printf("Key '%s' pressed\n", scancode_set[actual_key]);
        }
    }
    else
    {
        if (actual_key > 83)
        {
            printf("Key #%d released\n", actual_key);
        }
        else
        {
            printf("Key '%s' released\n", scancode_set[actual_key]);
        }
    }
}