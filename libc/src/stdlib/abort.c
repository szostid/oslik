#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tty.h>

__attribute__((__noreturn__)) void abort(void)
{
#if defined(__is_libk)
    terminal_clear(VGA_COLOR_BLUE);
    printf("kernel panic: abort()\n");
    __asm__ volatile("hlt");
#else
    printf("abort()\n");
#endif
    while (1)
    {
    }
    __builtin_unreachable();
}
