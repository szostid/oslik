#include "pic.h"
#include <ports.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// Initialization Command Word 1
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01

// Initialization Command Word 4
#define ICW4_8086 0x01

// End-of-Interrupt command code
#define PIC_EOI 0x20

static void io_wait(void)
{
    outb(0x80, 0);
}

static void outb_and_io_wait(uint16_t port, uint8_t val)
{
    outb(port, val);
    io_wait();
}

void setup_pic(void)
{
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // ICW1: Start the initialization sequence in cascade mode
    outb_and_io_wait(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb_and_io_wait(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // ICW2: Set the new vector offsets
    // PIC1 is at ports 32..40, PIC2 is at ports 40..48
    outb_and_io_wait(PIC1_DATA, 32);
    outb_and_io_wait(PIC2_DATA, 40);

    // ICW3: Tell the Master PIC there is a slave PIC at IRQ2 (0000 0100),
    // then tell the Slave PIC its cascade identity (0000 0010)
    outb_and_io_wait(PIC1_DATA, 4);
    outb_and_io_wait(PIC2_DATA, 2);

    // ICW4: Set 8086 mode
    outb_and_io_wait(PIC1_DATA, ICW4_8086);
    outb_and_io_wait(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_eoi(uint8_t irq)
{
    if (irq >= 8)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}