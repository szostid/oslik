#include <idt.h>
#include <stdarg.h>
#include <stdio.h>
#include <tty.h>

void start_kpanic()
{
    // When the kernel panics, we no longer want to receive any interrupts -
    // first of all, the kernel panic message must be written without any
    // interrupts, but also, the system is likely in an invalid state, where it
    // shouldn't execute anymore, and executing interrupts would in fact cause
    // code to execute
    isr_pause();

    set_active_tty(&kernel_tty);

    tty_clear(&kernel_tty, TTY_COLOR_BLUE);

    printf("-------- KERNEL PANIC --------\n\n");
}

__attribute__((__noreturn__)) void end_kpanic()
{
    printf("\n------------------------------");

    while (1)
    {
        __asm__ volatile("hlt");
    }
    __builtin_unreachable();
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
