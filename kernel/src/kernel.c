#include <gdt.h>
#include <idt.h>
#include <paging.h>
#include <panic.h>
#include <pic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tty.h>

void kernel_main(void)
{
    tty_initialize(&kernel_tty);
    setup_gdt();
    pic_remap();
    idt_init();
    setup_paging();

    printf("Hello, world!\n");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}