#include <input.h>
#include <panic.h>
#include <ports.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tty.h>

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

void tty_write_scratchpad(char *buf)
{
    tty_set_char_at(&kernel_tty, '>', 0, TTY_HEIGHT);
    tty_set_char_at(&kernel_tty, ' ', 1, TTY_HEIGHT);

    for (size_t i = 0; i < SCRATCHPAD_WIDTH; i++)
    {
        tty_set_char_at(&kernel_tty, buf[i], i + 2, TTY_HEIGHT);
    }

    tty_flush(&kernel_tty);
}

extern void run_tetris(void);

void handle_scratchpad(char *buf)
{
    if (memcmp(buf, "tetris", 6) == 0)
    {
        printf("Opening tetris\n");
        run_tetris();
        return;
    }

    printf("%s\n", (char *)(&scratchpad));
}

void write_scratchpad(keys_t key, bool was_pressed)
{
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
            handle_scratchpad(scratchpad);
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

        // TODO:
        // - KB_ERRCODE
        // - KB_Esc
        // - KB_Tab
        // - KB_Keypad_Asterisk
        // - KB_Keypad_Key7
        // - KB_Keypad_Key8
        // - KB_Keypad_Key9
        // - KB_Keypad_Minus
        // - KB_Keypad_Key4
        // - KB_Keypad_Key5
        // - KB_Keypad_Key6
        // - KB_Keypad_Plus
        // - KB_Keypad_Key1
        // - KB_Keypad_Key2
        // - KB_Keypad_Key3
        // - KB_Keypad_Key0
        // - KB_Keypad_Dot
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

    kernel_tty.cursor_col = scratchpad_ptr - scratchpad_start_idx + 1;
    kernel_tty.cursor_row = VGA_HEIGHT - 1;

    tty_write_scratchpad(&scratchpad[scratchpad_start_idx]);
}

void setup_input()
{
    kernel_tty.cursor_visible = true;
    kernel_tty.on_keypress = write_scratchpad;
}