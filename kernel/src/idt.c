#include <gdt.h>
#include <panic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <tty.h>

static const char *exception_labels[] = {
    "[0x00] Divide by Zero Exception",
    "[0x01] Debug Exception",
    "[0x02] Unhandled Non-maskable Interrupt",
    "[0x03] Breakpoint Exception",
    "[0x04] Overflow Exception",
    "[0x05] Bound Range Exceeded Exception",
    "[0x06] Invalid Opcode/Operand Exception",
    "[0x07] Device Unavailable Exception",
    "[0x08] Double Fault",
    "[0x09] Coprocessor Segment Overrun",
    "[0x0A] Invalid TSS Exception",
    "[0x0B] Absent Segment Exception",
    "[0x0C] Stack-segment Fault",
    "[0x0D] General Protection Fault",
    "[0x0E] Page Fault",
    "[0x0F] Inexplicable Error",
    "[0x10] x87 Floating Exception",
    "[0x11] Alignment Check",
    "[0x12] Machine Check",
    "[0x13] SIMD Floating Exception",
    "[0x14] Virtualized Exception",
    "[0x15] Control Protection Exception",
    "[0x16] Inexplicable Error",
    "[0x17] Inexplicable Error",
    "[0x18] Inexplicable Error",
    "[0x19] Inexplicable Error",
    "[0x1A] Inexplicable Error",
    "[0x1B] Inexplicable Error",
    "[0x1C] Hypervisor Intrusion Exception",
    "[0x1D] VMM Communications Exception",
    "[0x1E] Security Exception",
    "[0x1F] Inexplicable Error",
};

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

typedef struct registers
{
    uint32_t edi, esi, ebp, esp_dummy;
    uint32_t ebx, edx, ecx, eax;

    uint32_t int_no;
    uint32_t err_code;

    uint32_t eip, cs, eflags;
} interrupt_state_t;

void print_interrupt(interrupt_state_t *state)
{
    if (state->int_no > 0x1F)
    {
        printf("-> Unknown interrupt %d", state->int_no);
    }
    else
    {
        const char *exception_label = exception_labels[state->int_no];
        printf("-> %s", exception_label);
    }

    printf("\n\nCPU state:\n");
    printf("err_code: %d\n", state->err_code);
    printf("eip: %d\n", state->eip);
    printf("cs: %d\n", state->cs);
    printf("eflags: %d\n", state->eflags);
    printf("edi: %d\n", state->edi);
    printf("esi: %d\n", state->esi);
    printf("ebp: %d\n", state->ebp);
    printf("esp_dummy: %d\n", state->esp_dummy);
}

void fault_interrupt(interrupt_state_t *state)
{
    start_kpanic();

    printf("Exception interrupt received:\n");

    print_interrupt(state);

    printf("------------------------------\n");

    end_kpanic();
}

void interrupt_handler(interrupt_state_t *state)
{
    if (state->int_no < 32)
    {
        switch (state->int_no)
        {
        case 0:
        case 6:
        case 13:
            fault_interrupt(state);
            break;
        default:
            printf("------------------------------\n");
            printf("Interrupt received\n");
            print_interrupt(state);
            printf("------------------------------\n");
            break;
        }
    }
    else
    {
        // Handle hardware interrupts (IRQs) from devices like keyboard, timer,
        // etc. For now, we'll just print a message.
        printf("\n\nHardware IRQ Received: %d\n", state->int_no);
    }
}
