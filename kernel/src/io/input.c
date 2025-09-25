#include <panic.h>
#include <ports.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tty.h>

#define KEYBOARD_DATA_PORT 0x60

#define HANDLE_KEY(KEY, LW, HI)                                                \
    case (KEY):                                                                \
        if (is_shift_pressed())                                                \
        {                                                                      \
            scratchpad_write((HI));                                            \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            scratchpad_write((LW));                                            \
        }                                                                      \
        break;

// clang-format off

enum {
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
    KB_Keypad_Key9     = 0x49, // `9` / page up
    KB_Keypad_Minus    = 0x4a,
    KB_Keypad_Key4     = 0x4b, // `4` / left arrow
    KB_Keypad_Key5     = 0x4c, // `5`
    KB_Keypad_Key6     = 0x4d, // `6` / right arrow
    KB_Keypad_Plus     = 0x4e, // `+`
    KB_Keypad_Key1     = 0x4f, // `1` / end
    KB_Keypad_Key2     = 0x50, // `2` / down arrow
    KB_Keypad_Key3     = 0x51, // `3` / page down
    KB_Keypad_Key0     = 0x52, // `0` / insert
    KB_Keypad_Dot      = 0x53, // `.` / delete
} keys_t;

enum
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

static uint16_t modifiers = 0;

static inline bool is_any_modifier_present(uint16_t present_modifiers)
{
    return (modifiers & present_modifiers) != 0;
}

static inline void set_modifier_state(bool present, uint16_t modifier)
{
    if (present)
    {
        modifiers |= modifier;
    }
    else
    {
        modifiers &= ~modifier;
    }
}

bool is_shift_pressed()
{
    return is_any_modifier_present(MOD_SHIFT_L | MOD_SHIFT_R);
}

bool is_ctrl_pressed()
{
    return is_any_modifier_present(MOD_CTRL_L | MOD_CTRL_R);
}

bool is_alt_pressed()
{
    return is_any_modifier_present(MOD_ALT_L | MOD_ALT_R);
}

bool is_input_capitalized()
{
    bool shift = is_shift_pressed();
    bool caps = is_any_modifier_present(MOD_CAPS_LOCK);
    return (!caps && shift) || (caps && !shift);
}

/// Scratchpad.
///
/// NOTE: To ensure the scratchpad is always null-terminated, the scratchpad
/// buffer is one (zeroed) byte longer than the possible max user input.
static char scratchpad[257];
/// Pointer to the scratchpad character index that will be written next
static int scratchpad_ptr = 0;

void scratchpad_write(char c)
{
    if (scratchpad_ptr == 256)
    {
        return;
    }

    scratchpad[scratchpad_ptr++] = c;
}

void on_key_press()
{
    uint8_t scancode = inb(0x60);

    bool was_pressed = (scancode & 128) == 0;
    uint8_t key = scancode & (~128);

    if (!was_pressed)
    {
        switch (key)
        {
        case KB_LShift:
            set_modifier_state(false, MOD_SHIFT_L);
            break;
        case KB_RShift:
            set_modifier_state(false, MOD_SHIFT_R);
            break;
        case KB_LAlt:
            set_modifier_state(false, MOD_ALT_L);
            break;
        case KB_LCtrl:
            set_modifier_state(false, MOD_CTRL_L);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (key)
        {
            HANDLE_KEY(KB_Q, 'q', 'Q')
            HANDLE_KEY(KB_W, 'w', 'W')
            HANDLE_KEY(KB_E, 'e', 'E')
            HANDLE_KEY(KB_R, 'r', 'R')
            HANDLE_KEY(KB_T, 't', 'T')
            HANDLE_KEY(KB_Y, 'y', 'Y')
            HANDLE_KEY(KB_U, 'u', 'U')
            HANDLE_KEY(KB_I, 'i', 'I')
            HANDLE_KEY(KB_O, 'o', 'O')
            HANDLE_KEY(KB_P, 'p', 'P')
            HANDLE_KEY(KB_A, 'a', 'A')
            HANDLE_KEY(KB_S, 's', 'S')
            HANDLE_KEY(KB_D, 'd', 'D')
            HANDLE_KEY(KB_F, 'f', 'F')
            HANDLE_KEY(KB_G, 'g', 'G')
            HANDLE_KEY(KB_H, 'h', 'H')
            HANDLE_KEY(KB_J, 'j', 'J')
            HANDLE_KEY(KB_K, 'k', 'K')
            HANDLE_KEY(KB_L, 'l', 'L')
            HANDLE_KEY(KB_Z, 'z', 'Z')
            HANDLE_KEY(KB_X, 'x', 'X')
            HANDLE_KEY(KB_C, 'c', 'C')
            HANDLE_KEY(KB_V, 'v', 'V')
            HANDLE_KEY(KB_B, 'b', 'B')
            HANDLE_KEY(KB_N, 'n', 'N')
            HANDLE_KEY(KB_M, 'm', 'M')

            HANDLE_KEY(KB_Key1, '1', '!')
            HANDLE_KEY(KB_Key2, '2', '@')
            HANDLE_KEY(KB_Key3, '3', '#')
            HANDLE_KEY(KB_Key4, '4', '$')
            HANDLE_KEY(KB_Key5, '5', '%')
            HANDLE_KEY(KB_Key6, '6', '^')
            HANDLE_KEY(KB_Key7, '7', '&')
            HANDLE_KEY(KB_Key8, '8', '*')
            HANDLE_KEY(KB_Key9, '9', '(')
            HANDLE_KEY(KB_Key0, '0', ')')

            HANDLE_KEY(KB_Minus, '-', '_')
            HANDLE_KEY(KB_Equals, '=', '+')
            HANDLE_KEY(KB_LBracket, '[', '{')
            HANDLE_KEY(KB_RBracket, ']', '}')
            HANDLE_KEY(KB_Semicolon, ';', ':')
            HANDLE_KEY(KB_Quote, '\'', '\"')
            HANDLE_KEY(KB_Tilde, '`', '~')
            HANDLE_KEY(KB_Comma, ',', '<')
            HANDLE_KEY(KB_Dot, '.', '>')
            HANDLE_KEY(KB_Slash, '/', '?')

            HANDLE_KEY(KB_Space, ' ', ' ')

        case KB_F1:
        case KB_F2:
        case KB_F3:
        case KB_F4:
        case KB_F5:
        case KB_F6:
        case KB_F7:
        case KB_F8:
        case KB_F9:
            printf("F%d pressed\n", key - 0x3a);
            break;

        case KB_F10:
            kpanic("F10 pressed\n");
            break;

        case KB_Backspace:
            if (scratchpad_ptr > 0)
            {
                scratchpad[scratchpad_ptr - 1] = 0;
                scratchpad_ptr--;
            }
            break;

        case KB_Enter:
            printf("%s\n", (char *)(&scratchpad));
            memset(scratchpad, 0, sizeof(scratchpad));
            scratchpad_ptr = 0;
            break;

        case KB_LShift:
            set_modifier_state(true, MOD_SHIFT_L);
            break;
        case KB_RShift:
            set_modifier_state(true, MOD_SHIFT_R);
            break;
        case KB_LAlt:
            set_modifier_state(true, MOD_ALT_L);
            break;
        case KB_LCtrl:
            set_modifier_state(true, MOD_CTRL_L);
            break;
        case KB_CapsLock:
            modifiers ^= MOD_CAPS_LOCK;
            break;
        case KB_NumLock:
            modifiers ^= MOD_NUM_LOCK;
            break;
        case KB_ScrollLock:
            modifiers ^= MOD_SCROLL_LOCK;
            break;

        default:
            printf("key %d\n", key);
        }
    }

    int scratchpad_start_idx;

    if (scratchpad_ptr < SCRATCHPAD_WIDTH)
    {
        scratchpad_start_idx = 0;
    }
    else
    {
        scratchpad_start_idx = scratchpad_ptr - SCRATCHPAD_WIDTH;
    }

    set_scratchpad(&scratchpad[scratchpad_start_idx]);
}