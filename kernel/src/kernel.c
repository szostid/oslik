#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tty.h>

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