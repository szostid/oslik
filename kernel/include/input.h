#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stdint.h>

void setup_input();

// clang-format off

typedef enum {
    KB_ERRCODE         = 0x00,
    KB_Esc             = 0x01,
    KB_Key1            = 0x02, // `1` / `!`
    KB_Key2            = 0x03, // `2` / `@`
    KB_Key3            = 0x04, // `3` / `#`
    KB_Key4            = 0x05, // `4` / `$`
    KB_Key5            = 0x06, // `5` / `%`
    KB_Key6            = 0x07, // `6` / `^`
    KB_Key7            = 0x08, // `7` / `&`
    KB_Key8            = 0x09, // `8` / `*`
    KB_Key9            = 0x0a, // `9` / `(`
    KB_Key0            = 0x0b, // `0` / `)`
    KB_Minus           = 0x0c, // `-` / `_`
    KB_Equals          = 0x0d, // `=` / `+`
    KB_Backspace       = 0x0e,
    KB_Tab             = 0x0f,
    KB_Q               = 0x10,
    KB_W               = 0x11,
    KB_E               = 0x12,
    KB_R               = 0x13,
    KB_T               = 0x14,
    KB_Y               = 0x15,
    KB_U               = 0x16,
    KB_I               = 0x17,
    KB_O               = 0x18,
    KB_P               = 0x19,
    KB_LBracket        = 0x1a, // `[` / `{`
    KB_RBracket        = 0x1b, // `]` / `}`
    KB_Enter           = 0x1c,
    KB_LCtrl           = 0x1d,
    KB_A               = 0x1e,
    KB_S               = 0x1f,
    KB_D               = 0x20,
    KB_F               = 0x21,
    KB_G               = 0x22,
    KB_H               = 0x23,
    KB_J               = 0x24,
    KB_K               = 0x25,
    KB_L               = 0x26,
    KB_Semicolon       = 0x27, // `;` / `:`
    KB_Quote           = 0x28, // `'` / `"`
    KB_Tilde           = 0x29, // ``` / `~`
    KB_LShift          = 0x2a,
    KB_Backslash       = 0x2b,
    KB_Z               = 0x2c,
    KB_X               = 0x2d,
    KB_C               = 0x2e,
    KB_V               = 0x2f,
    KB_B               = 0x30,
    KB_N               = 0x31,
    KB_M               = 0x32,
    KB_Comma           = 0x33, // `,` / `<`
    KB_Dot             = 0x34, // `.` / `>`
    KB_Slash           = 0x35, // `/` / `?`
    KB_RShift          = 0x36,
    KB_Keypad_Asterisk = 0x37, // `*` on a keypad
    KB_LAlt            = 0x38,
    KB_Space           = 0x39,
    KB_CapsLock        = 0x3a,
    KB_F1              = 0x3b,
    KB_F2              = 0x3c,
    KB_F3              = 0x3d,
    KB_F4              = 0x3e,
    KB_F5              = 0x3f,
    KB_F6              = 0x40,
    KB_F7              = 0x41,
    KB_F8              = 0x42,
    KB_F9              = 0x43,
    KB_F10             = 0x44,
    KB_NumLock         = 0x45,
    KB_ScrollLock      = 0x46,
    KB_Keypad_Key7     = 0x47, // `7` / home
    KB_Keypad_Key8     = 0x48, // `8` / up arrow
    KB_Arrow_Up        = 0x48, // `8` / up arrow
    KB_Keypad_Key9     = 0x49, // `9` / page up
    KB_Keypad_Minus    = 0x4a,
    KB_Keypad_Key4     = 0x4b, // `4` / left arrow
    KB_Arrow_Left      = 0x4b, // `4` / left arrow
    KB_Keypad_Key5     = 0x4c, // `5`
    KB_Keypad_Key6     = 0x4d, // `6` / right arrow
    KB_Arrow_Right     = 0x4d, // `6` / right arrow
    KB_Keypad_Plus     = 0x4e, // `+`
    KB_Keypad_Key1     = 0x4f, // `1` / end
    KB_Keypad_Key2     = 0x50, // `2` / down arrow
    KB_Arrow_Down      = 0x50, // `2` / down arrow
    KB_Keypad_Key3     = 0x51, // `3` / page down
    KB_Keypad_Key0     = 0x52, // `0` / insert
    KB_Keypad_Dot      = 0x53, // `.` / delete
} keys_t;

typedef enum
{
    MOD_SHIFT_L     = 1 << 0,
    MOD_SHIFT_R     = 1 << 1,
    MOD_ALT_L       = 1 << 2,
    MOD_ALT_R       = 1 << 3,
    MOD_CTRL_L      = 1 << 4,
    MOD_CTRL_R      = 1 << 5,
    MOD_CAPS_LOCK   = 1 << 6,
    MOD_NUM_LOCK    = 1 << 7,
    MOD_SCROLL_LOCK = 1 << 8,
} modifiers_t;

// clang-format on

#endif