.intel_syntax noprefix

.global isr_stub_table
.extern interrupt_handler

.macro isr_no_err_stub num
isr_stub_\num:
    # Disable interrupts now that we're handling one. They will be re-enabled
    # when we return from the interrupt and the CPU loads the old EFLAGS.
    cli

    # We push the emulated error code (the one which would've otherwise been
    # visible to the `ist_err_stub`, pushed by the CPU), the interrupt code,
    # and the register state.
    push 0
    push \num
    pushad

    # C code following the sysV ABI requires DF to be clear on function entry
    cld

    # We push the stack pointer, which esentially points to everything we've
    # just pushed onto the stack in this handler
    push esp
    call interrupt_handler
    # Pop the stack pointer
    add esp, 4

    # We pop the register state, the interrupt code and the emulated error code
    # that we've pushed at the beginning of this function
    popad
    add esp, 8

    iret
.endm

.macro isr_err_stub num
isr_stub_\num:
    # Disable interrupts now that we're handling one. They will be re-enabled
    # when we return from the interrupt and the CPU loads the old EFLAGS.
    cli

    # We push the interrupt code, the register state and error register state
    push \num
    pushad

    # C code following the sysV ABI requires DF to be clear on function entry
    cld

    # We push the stack pointer, which esentially points to everything we've
    # just pushed onto the stack in this handler
    push esp
    call interrupt_handler
    # Pop the stack pointer
    add esp, 4

    # We pop the register state and the interrupt code that we've pushed at the
    # beginning of this function. We also have to pop the error code that was
    # pushed by the CPU (`iret` does handle CS, EFLAGS and EIP but it does not
    # handle the error code)
    popad
    add esp, 8

    iret
.endm

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
isr_no_err_stub 32
isr_no_err_stub 33
isr_no_err_stub 34
isr_no_err_stub 35
isr_no_err_stub 36
isr_no_err_stub 37
isr_no_err_stub 38
isr_no_err_stub 39
isr_no_err_stub 40
isr_no_err_stub 41
isr_no_err_stub 42
isr_no_err_stub 43
isr_no_err_stub 44
isr_no_err_stub 45
isr_no_err_stub 46
isr_no_err_stub 47

isr_stub_table:
    .long isr_stub_0
    .long isr_stub_1
    .long isr_stub_2
    .long isr_stub_3
    .long isr_stub_4
    .long isr_stub_5
    .long isr_stub_6
    .long isr_stub_7
    .long isr_stub_8
    .long isr_stub_9
    .long isr_stub_10
    .long isr_stub_11
    .long isr_stub_12
    .long isr_stub_13
    .long isr_stub_14
    .long isr_stub_15
    .long isr_stub_16
    .long isr_stub_17
    .long isr_stub_18
    .long isr_stub_19
    .long isr_stub_20
    .long isr_stub_21
    .long isr_stub_22
    .long isr_stub_23
    .long isr_stub_24
    .long isr_stub_25
    .long isr_stub_26
    .long isr_stub_27
    .long isr_stub_28
    .long isr_stub_29
    .long isr_stub_30
    .long isr_stub_31
    .long isr_stub_32
    .long isr_stub_33
    .long isr_stub_34
    .long isr_stub_35
    .long isr_stub_36
    .long isr_stub_37
    .long isr_stub_38
    .long isr_stub_39
    .long isr_stub_40
    .long isr_stub_41
    .long isr_stub_42
    .long isr_stub_43
    .long isr_stub_44
    .long isr_stub_45
    .long isr_stub_46
    .long isr_stub_47