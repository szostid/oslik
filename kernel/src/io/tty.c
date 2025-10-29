#include <idt.h>
#include <ports.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <tty.h>

#ifdef SERIAL_WRITE_TTY
#include <serial.h>
#endif

#define VGA_MEMORY 0xB8000

tty_t kernel_tty;

tty_t *active_tty = &kernel_tty;

static inline void vga_set_cursor_position(size_t x, size_t y)
{
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static inline void vga_set_cursor_visible(bool visible)
{
    outb(0x3D4, 0x0A);
    uint8_t cursor_start = inb(0x3D5);

    if (visible)
    {
        cursor_start &= ~0x20;
    }
    else
    {
        cursor_start |= 0x20;
    }

    outb(0x3D4, 0x0A);
    outb(0x3D5, cursor_start);
}

static inline uint16_t vga_entry_pack(terminal_entry_t terminal_entry)
{
    return (uint16_t)(terminal_entry.character) |
           (uint16_t)(terminal_entry.color.background) << 12 |
           (uint16_t)(terminal_entry.color.foreground) << 8;
}

static inline terminal_entry_t vga_entry_unpack(uint16_t packed_entry)
{
    terminal_entry_t entry = {
        .character = packed_entry & 0xFF,
        .color =
            {
                .foreground = (packed_entry >> 8) & 0xF,
                .background = (packed_entry >> 12) & 0xF,
            },
    };

    return entry;
}

static size_t vga_index(size_t x, size_t y)
{
    return y * VGA_WIDTH + x;
}

void tty_set_color(tty_t *tty, terminal_entry_color_t color)
{
    tty->color = color;
}

void tty_set_entry_at(tty_t *tty, terminal_entry_t entry, size_t x, size_t y)
{
    const size_t index = vga_index(x, y);

    tty->buffer[index] = vga_entry_pack(entry);
}

void tty_set_char_at(tty_t *tty, char c, size_t x, size_t y)
{
    const terminal_entry_t entry = {
        .character = c,
        .color = tty->color,
    };

    tty_set_entry_at(tty, entry, x, y);
}

terminal_entry_t tty_read_at(tty_t *tty, size_t x, size_t y)
{
    const size_t index = vga_index(x, y);
    return vga_entry_unpack(tty->buffer[index]);
}

void tty_move_up(tty_t *tty)
{
    for (size_t y = 1; y < TTY_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const terminal_entry_t entry = tty_read_at(tty, x, y);

            tty_set_entry_at(tty, entry, x, y - 1);
        }
    }

    const terminal_entry_t blank_terminal_entry = {
        .character = ' ',
        .color = tty->color,
    };

    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        tty_set_entry_at(tty, blank_terminal_entry, x, TTY_HEIGHT - 1);
    }
}

void tty_next_line(tty_t *tty)
{
    tty->cursor_col = 0;
    tty->cursor_row += 1;

    if (tty->cursor_row == TTY_HEIGHT)
    {
        tty->cursor_row = TTY_HEIGHT - 1;
        tty_move_up(tty);
    }
}

void tty_next_char(tty_t *tty)
{
    tty->cursor_col += 1;

    if (tty->cursor_col == VGA_WIDTH)
    {
        tty_next_line(tty);
    }
}

void tty_put_entry(tty_t *tty, terminal_entry_t entry)
{
#ifdef SERIAL_WRITE_TTY
    write_serial((char)entry.character);
#endif

    if (entry.character == '\n')
    {
        tty_next_line(tty);
        return;
    }

    tty_set_entry_at(tty, entry, tty->cursor_col, tty->cursor_row);

    tty_next_char(tty);
}

static void tty_put_char(tty_t *tty, const char data)
{
    const terminal_entry_t entry = {
        .character = data,
        .color = tty->color,
    };

    tty_put_entry(tty, entry);
}

void tty_write(tty_t *tty, const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        tty_put_char(tty, data[i]);
    }
}

void tty_write_string(tty_t *tty, const char *data)
{
    tty_write(tty, data, strlen(data));
}

void tty_clear(tty_t *tty, terminal_color_t background)
{
    tty->cursor_row = 0;
    tty->cursor_col = 0;

    terminal_entry_color_t default_color = {
        .background = background,
        .foreground = TTY_COLOR_WHITE,
    };

    tty->color = default_color;

    const terminal_entry_t blank_terminal_entry = {
        .character = ' ',
        .color = default_color,
    };

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            tty_set_entry_at(tty, blank_terminal_entry, x, y);
        }
    }
}

void tty_initialize(tty_t *tty)
{
    tty_clear(tty, TTY_COLOR_BLACK);
    tty_flush(tty);
}

void tty_flush(tty_t *tty)
{
    if (active_tty != tty)
    {
        return;
    }

    isr_pause();

    memcpy((void *)VGA_MEMORY, tty->buffer, sizeof(uint16_t[BUFFER_SIZE]));
    vga_set_cursor_position(tty->cursor_col, tty->cursor_row);
    vga_set_cursor_visible(tty->cursor_visible);

    isr_resume();
}

void set_active_tty(tty_t *tty)
{
    active_tty = tty;
    tty_flush(tty);
}

tty_t *get_active_tty()
{
    return active_tty;
}