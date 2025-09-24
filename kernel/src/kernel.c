#include <gdt.h>
#include <idt.h>
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
    idt_init();

    printf("Hello, hernel world\n");
    printf("Newline test!\n");

    terminal_move_up();

    asm volatile("int $0x3");
    asm volatile("int $0x4");

    // kpanic("invalid stuff");
}