#include <stdarg.h>
#include <stdio.h>
#include <tty.h>

void start_kpanic()
{
    // __asm__ volatile("cli");

    terminal_clear(VGA_COLOR_BLUE);
    printf("-------- KERNEL PANIC --------\n\n");
}

__attribute__((__noreturn__)) void kpanic(const char *__restrict format, ...)
{
    start_kpanic();

    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    end_kpanic();
}

__attribute__((__noreturn__)) void end_kpanic()
{
    while (1)
    {
        __asm__ volatile("hlt");
    }
    __builtin_unreachable();
}