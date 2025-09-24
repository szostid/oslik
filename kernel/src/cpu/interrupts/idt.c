#include <gdt.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    /// @brief The lower 16 bits of the ISR's address
    uint16_t isr_low;
    /// @brief The GDT segment selector that the CPU will load into CS before
    /// calling the ISR
    uint16_t kernel_cs;
    /// @brief Set to zero
    uint8_t reserved;
    /// @brief Type and attributes; see the IDT page
    uint8_t attributes;
    /// @brief The higher 16 bits of the ISR's address
    uint16_t isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

#define IDT_GATE_INTERRUPT 0x8E
#define IDT_GATE_TRAP 0x8F
#define IDT_GATE_TASK 0x85

__attribute__((aligned(0x10))) static idt_entry_t idt[256];
static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = KERNEL_CS;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

#define IDT_MAX_DESCRIPTORS 32

static bool vectors[IDT_MAX_DESCRIPTORS];
extern void *isr_stub_table[];

void idt_init()
{
    idtr.base = (uintptr_t)(&idt[0]);
    idtr.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1);

    for (uint8_t vector = 0; vector < 32; vector++)
    {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_GATE_INTERRUPT);
        vectors[vector] = true;
    }

    // load the new IDT
    __asm__ volatile("lidt %0" : : "m"(idtr));
    // set the interrupt flag
    __asm__ volatile("sti");
}
