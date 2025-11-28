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
        if (is_shift_pressed(scratchpad))                                      \
        {                                                                      \
            scratchpad_write(scratchpad, (HI));                                \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            scratchpad_write(scratchpad, (LW));                                \
        }                                                                      \
        break;

typedef struct
{
    /// @brief Scratchpad buffer + zero byte at the end for null termination
    char data[257];
    /// @brief The index in the scratchpad at which the
    ///        next character will be written
    int next_insert_ptr;
    /// @brief Currently pressed modifiers
    uint16_t modifiers;
    /// @brief Registered TTY commands
    scratchpad_cmd_t commands[128];
    int command_count;
} scratchpad_t;

static inline bool is_any_modifier_present(scratchpad_t *scratchpad,
                                           uint16_t present_modifiers)
{
    return (scratchpad->modifiers & present_modifiers) != 0;
}

static inline void set_modifier_state(scratchpad_t *scratchpad, bool present,
                                      uint16_t modifier)
{
    if (present)
    {
        scratchpad->modifiers |= modifier;
    }
    else
    {
        scratchpad->modifiers &= ~modifier;
    }
}

bool is_shift_pressed(scratchpad_t *scratchpad)
{
    return is_any_modifier_present(scratchpad, MOD_SHIFT_L | MOD_SHIFT_R);
}

bool is_ctrl_pressed(scratchpad_t *scratchpad)
{
    return is_any_modifier_present(scratchpad, MOD_CTRL_L | MOD_CTRL_R);
}

bool is_alt_pressed(scratchpad_t *scratchpad)
{
    return is_any_modifier_present(scratchpad, MOD_ALT_L | MOD_ALT_R);
}

bool is_input_capitalized(scratchpad_t *scratchpad)
{
    bool shift = is_shift_pressed(scratchpad);
    bool caps = is_any_modifier_present(scratchpad, MOD_CAPS_LOCK);
    return (!caps && shift) || (caps && !shift);
}

void scratchpad_add_command(scratchpad_t *scratchpad, scratchpad_cmd_t cmd)
{
    scratchpad->commands[scratchpad->command_count++] = cmd;
}

void scratchpad_write(scratchpad_t *scratchpad, char c)
{
    if (scratchpad->next_insert_ptr == 256)
    {
        return;
    }

    scratchpad->data[scratchpad->next_insert_ptr++] = c;
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

void handle_scratchpad(scratchpad_t *scratchpad)
{
    for (int i = 0; i < scratchpad->command_count; i++)
    {
        scratchpad_cmd_t *cmd = &scratchpad->commands[i];

        if (memcmp(&scratchpad->data, cmd->name, cmd->name_len) == 0)
        {
            (cmd->callback)();
            goto cleanup;
        }
    }

    printf("Unknown command: %s\n", &scratchpad->data);

cleanup:
    memset(&scratchpad->data, 0, sizeof(scratchpad->data));
    scratchpad->next_insert_ptr = 0;
}

void write_scratchpad(keys_t key, bool was_pressed, void *data)
{
    scratchpad_t *scratchpad = (scratchpad_t *)data;

    if (!was_pressed)
    {
        switch (key)
        {
        case KB_LShift:
            set_modifier_state(scratchpad, false, MOD_SHIFT_L);
            break;
        case KB_RShift:
            set_modifier_state(scratchpad, false, MOD_SHIFT_R);
            break;
        case KB_LAlt:
            set_modifier_state(scratchpad, false, MOD_ALT_L);
            break;
        case KB_LCtrl:
            set_modifier_state(scratchpad, false, MOD_CTRL_L);
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
            if (scratchpad->next_insert_ptr > 0)
            {
                scratchpad->data[--scratchpad->next_insert_ptr] = 0;
            }
            break;

        case KB_Enter:
            handle_scratchpad(scratchpad);
            break;

        case KB_LShift:
            set_modifier_state(scratchpad, true, MOD_SHIFT_L);
            break;
        case KB_RShift:
            set_modifier_state(scratchpad, true, MOD_SHIFT_R);
            break;
        case KB_LAlt:
            set_modifier_state(scratchpad, true, MOD_ALT_L);
            break;
        case KB_LCtrl:
            set_modifier_state(scratchpad, true, MOD_CTRL_L);
            break;
        case KB_CapsLock:
            scratchpad->modifiers ^= MOD_CAPS_LOCK;
            break;
        case KB_NumLock:
            scratchpad->modifiers ^= MOD_NUM_LOCK;
            break;
        case KB_ScrollLock:
            scratchpad->modifiers ^= MOD_SCROLL_LOCK;
            break;

        default:
            printf("Unhandleable key: %d\n", key);
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

    if (scratchpad->next_insert_ptr < SCRATCHPAD_WIDTH)
    {
        scratchpad_start_idx = 0;
    }
    else
    {
        scratchpad_start_idx = scratchpad->next_insert_ptr - SCRATCHPAD_WIDTH;
    }

    // this breaks printing because tty prints characters at cursor position

    // kernel_tty.cursor_col =
    //     scratchpad->next_insert_ptr - scratchpad_start_idx + 2;
    // kernel_tty.cursor_row = VGA_HEIGHT - 1;

    tty_write_scratchpad((char *)&scratchpad->data[scratchpad_start_idx]);
}

scratchpad_t scratchpad;

/// @brief Sets up the writing to scratchpad on `kernel_tty`
void setup_input()
{
    tty_initialize(&kernel_tty);
    kernel_tty.cursor_visible = false;
    tty_set_keypress_callback(&kernel_tty, write_scratchpad, &scratchpad);
}

void add_command(scratchpad_cmd_t cmd)
{
    scratchpad_add_command(&scratchpad, cmd);
}