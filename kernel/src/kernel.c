#include "tty.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len] != 0)
    {
        len++;
    }

    return len;
}

void kernel_main(void)
{
    terminal_initialize();

    terminal_entry_color_t color = {
        .background = VGA_COLOR_CYAN,
        .foreground = VGA_COLOR_MAGENTA,
    };

    terminal_write_string("Hello, kernel World!\n", color);
    terminal_write_string("Newline test!\n", color);

    terminal_move_up();
}