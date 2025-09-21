#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tty.h>

__attribute__((__noreturn__)) void abort(void)
{
#if defined(__is_libk)
    terminal_clear(VGA_COLOR_BLUE);
    printf("kernel panic: abort()\n");
    asm volatile("hlt");
#else
    printf("abort()\n");
#endif
    while (1)
    {
    }
    __builtin_unreachable();
}

__attribute__((__noreturn__)) void panic(const char *__restrict format, ...)
{
#if defined(__is_libk)
    terminal_clear(VGA_COLOR_BLUE);

    printf("kernel panic: ");

    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    asm volatile("hlt");
#else
    printf("abort()\n");
#endif
    while (1)
    {
    }
    __builtin_unreachable();
}
