#include <gdt.h>
#include <idt.h>
#include <input.h>
#include <paging.h>
#include <panic.h>
#include <pic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tty.h>

extern void run_tetris(void);

void kernel_main(void)
{
    tty_initialize(&kernel_tty);
    setup_gdt();
    pic_remap();
    idt_init();
    setup_paging();
    setup_input();

    printf("Hello, world!\n");

    run_tetris();

    // while (1)
    // {
    //     __asm__ volatile("hlt");
    // }
}