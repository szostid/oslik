#include <stdint.h>
#include <stdio.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t first_page_table2[1024] __attribute__((aligned(4096)));

#define PAGE_NOT_PRESENT (0)
#define PAGE_PRESENT (1 << 0)

#define PAGE_READ_ONLY (0)
#define PAGE_WRITEABLE (1 << 1)

#define PAGE_SUPERVISOR_ONLY (0)
#define PAGE_USER_ACCESSIBLE (1 << 2)

extern void loadPageDirectory(unsigned int *);
extern void enablePaging();

extern uint32_t endkernel;

void setup_paging()
{
    for (int i = 0; i < 1024; i++)
    {
        // Kernel-mode only, write enabled, not present
        page_directory[i] =
            PAGE_WRITEABLE | PAGE_SUPERVISOR_ONLY | PAGE_NOT_PRESENT;
    }

    for (int i = 0; i < 1024; i++)
    {
        first_page_table[i] =
            (i * 0x1000) | PAGE_WRITEABLE | PAGE_SUPERVISOR_ONLY | PAGE_PRESENT;
    }

    page_directory[0] = ((unsigned int)first_page_table) | 3;

    loadPageDirectory((unsigned int *)page_directory);
    enablePaging();
}