#include <panic.h>
#include <pic.h>
#include <ports.h>
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
    uint32_t edi, esi, ebp, esp_dummy;
    uint32_t ebx, edx, ecx, eax;

    uint32_t int_no;
    uint32_t err_code;

    uint32_t eip, cs, eflags;
} interrupt_state_t;

void handle_hw_interrupt(int32_t int_no)
{
    switch (int_no)
    {
    case 0:
        break;
    case 1:
        uint8_t scancode = inb(0x60);

        bool was_pressed = (scancode & 128) == 0;
        uint8_t key = scancode & (~128);

        tty_t *active_tty = get_active_tty();

        if (active_tty->on_keypress)
        {
            // the user callback may enter an infinite loop,
            // so we should send an EOI signal to the PIC before
            // calling the user callback. we will return early
            // to avoid the usual call to pic_eoi at the end of
            // this function.
            //
            // it shouldn't be much of an issue if another keypress
            // is received when handling keypresses.
            pic_eoi(int_no);

            active_tty->on_keypress(key, was_pressed,
                                    active_tty->keypress_callback_data);

            return;
        }

        break;
    default:
        printf("Hardware interrupt #%d received\n", int_no);
        break;
    }

    pic_eoi(int_no);
}

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
        handle_hw_interrupt(state->int_no - 32);
    }
}
