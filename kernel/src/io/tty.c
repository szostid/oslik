#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <tty.h>

#ifdef SERIAL_WRITE_TTY
#include <serial.h>
#endif

#define VGA_MEMORY 0xB8000

// lowest row is for the scratchpad

size_t terminal_row;
size_t terminal_column;
terminal_entry_color_t terminal_color;
uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

static inline terminal_entry_t blank_entry(void)
{
    terminal_entry_t entry = {
        .character = ' ',
        .color = terminal_color,
    };

    return entry;
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

static size_t terminal_pos_idx(size_t x, size_t y)
{
    return y * VGA_WIDTH + x;
}

void terminal_set_color(terminal_entry_color_t color)
{
    terminal_color = color;
}

void terminal_set_entry_at(terminal_entry_t entry, size_t x, size_t y)
{
    const size_t index = terminal_pos_idx(x, y);
    terminal_buffer[index] = vga_entry_pack(entry);
}

void terminal_set_char_at(char c, size_t x, size_t y)
{
    const terminal_entry_t entry = {
        .character = c,
        .color = terminal_color,
    };

    terminal_set_entry_at(entry, x, y);
}

terminal_entry_t terminal_read_at(size_t x, size_t y)
{
    const size_t index = terminal_pos_idx(x, y);
    return vga_entry_unpack(terminal_buffer[index]);
}

void terminal_move_up()
{
    for (size_t y = 1; y < TTY_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const terminal_entry_t entry = terminal_read_at(x, y);

            terminal_set_entry_at(entry, x, y - 1);
        }
    }

    const terminal_entry_t blank_terminal_entry = blank_entry();

    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        terminal_set_entry_at(blank_terminal_entry, x, TTY_HEIGHT - 1);
    }
}

void terminal_next_line()
{
    terminal_column = 0;
    terminal_row += 1;

    if (terminal_row == TTY_HEIGHT)
    {
        terminal_row = TTY_HEIGHT - 1;
        terminal_move_up();
    }
}

void terminal_next_char()
{
    terminal_column += 1;

    if (terminal_column == VGA_WIDTH)
    {
        terminal_next_line();
    }
}

void terminal_put_entry(terminal_entry_t entry)
{
#ifdef SERIAL_WRITE_TTY
    write_serial((char)entry.character);
#endif

    if (entry.character == '\n')
    {
        terminal_next_line();
        return;
    }

    terminal_set_entry_at(entry, terminal_column, terminal_row);

    terminal_next_char();
}

static void terminal_put_char(const char data)
{
    const terminal_entry_t entry = {
        .character = data,
        .color = terminal_color,
    };

    terminal_put_entry(entry);
}

void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        terminal_put_char(data[i]);
    }
}

void terminal_write_string(const char *data)
{
    terminal_write(data, strlen(data));
}

void terminal_clear(terminal_color_t background)
{
    terminal_row = 0;
    terminal_column = 0;

    terminal_entry_color_t default_color = {
        .background = background,
        .foreground = VGA_COLOR_WHITE,
    };

    terminal_color = default_color;

    const terminal_entry_t blank_terminal_entry = blank_entry();

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            terminal_set_entry_at(blank_terminal_entry, x, y);
        }
    }
}

void set_scratchpad(char *buf)
{
    terminal_set_char_at('>', 0, TTY_HEIGHT);
    terminal_set_char_at(' ', 1, TTY_HEIGHT);

    for (size_t i = 0; i < SCRATCHPAD_WIDTH; i++)
    {
        terminal_set_char_at(buf[i], i + 2, TTY_HEIGHT);
    }
}

void terminal_initialize(void)
{
    terminal_clear(VGA_COLOR_BLACK);
}