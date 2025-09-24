#ifndef GDT_H
#define GDT_H

void setup_gdt(void);

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#endif