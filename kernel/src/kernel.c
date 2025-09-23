#include <gdt.h>
#include <panic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tty.h>

void kernel_main(void)
{
    terminal_initialize();
    setup_gdt();

    printf("Hello, hernel world\n");
    printf("Newline test!\n");

    terminal_move_up();

    kpanic("invalid stuff");
}