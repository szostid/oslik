#include <gdt.h>
#include <idt.h>
#include <input.h>
#include <paging.h>
#include <pic.h>
#include <stdio.h>

extern void init_tetris();
extern void init_pong();

void kernel_main(void)
{
    setup_input();
    setup_gdt();
    setup_pic();
    setup_idt();
    setup_paging();

    init_tetris();
    init_pong();

    printf("Hello, world!\n");
    printf("Hello, world1!\n");
    printf("Hello, world2!\n");
    printf("Hello, world3!\n");

    while (1)
    {
        __asm__ volatile("hlt");
    }
}