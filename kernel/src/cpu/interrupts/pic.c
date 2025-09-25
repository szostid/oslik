#include "pic.h"
#include <ports.h> // You'll need a header for outb() and inb()

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

/*
Helper function to introduce a small delay. This is necessary because we are
communicating with very old hardware (the PIC chip) that needs a moment
to process each command.
*/
static void io_wait(void)
{
    // This is a common way to create a short I/O delay.
    outb(0x80, 0);
}

void pic_remap(void)
{
    // --- Remap the PICs ---
    // The PIC vectors originally conflicted with CPU exceptions.
    // We are remapping them to vectors 32..=47.

    // Save current masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // Start the initialization sequence in cascade mode
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Set the new vector offsets
    outb(PIC1_DATA, 32); // Master PIC starts at vector 32
    io_wait();
    outb(PIC2_DATA, 40); // Slave PIC starts at vector 40
    io_wait();

    // ICW3: Tell Master PIC there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    io_wait();
    // ICW3: Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 2);
    io_wait();

    // ICW4: Set 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_eoi(uint8_t irq)
{
    // If the IRQ came from the slave PIC, we must send an EOI
    // to both the slave and the master.
    if (irq >= 8)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    // In all cases, send an EOI to the master.
    outb(PIC1_COMMAND, PIC_EOI);
}