#include <stdint.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t first_page_table[1024] __attribute__((aligned(4096)));

#define PAGE_PRESENT (1 << 0)

#define PAGE_WRITEABLE (1 << 1)

#define PAGE_USER_ACCESSIBLE (1 << 2)

// This should go outside any function..
extern void loadPageDirectory(unsigned int *);
extern void enablePaging();

void setup_paging()
{
    for (int i = 0; i < 1024; i++)
    {
        // Kernel-mode only, write enabled, not present
        page_directory[i] = 0x2;
    }

    for (int i = 0; i < 1024; i++)
    {
        first_page_table[i] = (i * 0x1000) | 3;
    }

    page_directory[0] = ((unsigned int)first_page_table) | 3;

    loadPageDirectory((unsigned int *)page_directory);
    enablePaging();
}