#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tty.h>

void kernel_main(void)
{
    terminal_initialize();

    printf("Hello, hernel world\n");
    printf("Newline test!\n");

    terminal_move_up();

    panic("invalid stuff");
}