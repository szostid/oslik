#include <stdarg.h>
#include <stdio.h>
#include <tty.h>

__attribute__((__noreturn__)) void kpanic(const char *__restrict format, ...)
{
    terminal_clear(VGA_COLOR_BLUE);

    printf("kernel panic: ");

    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    __asm__ volatile("hlt");

    while (1)
    {
    }
    __builtin_unreachable();
}
