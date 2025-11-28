#include <gdt.h>
#include <idt.h>
#include <input.h>
#include <paging.h>
#include <pic.h>
#include <stdio.h>

void kernel_main(void)
{
    setup_input();
    setup_gdt();
    setup_pic();
    setup_idt();
    setup_paging();

    printf("Hello, world!\n");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}