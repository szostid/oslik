#include <gdt.h>
#include <idt.h>
#include <panic.h>
#include <pic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tty.h>

void kernel_main(void)
{
    terminal_initialize();
    setup_gdt();
    pic_remap();
    idt_init();

    printf("Hello, world!\n");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}