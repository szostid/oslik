#ifndef IDT_H
#define IDT_H

/// @brief Sets up the IDT (interrupt descriptor table),
///        letting the CPU execute interrupts again.
void setup_idt(void);

/// @brief Pauses ISRs (interrup service routines).
///        No interrupts will be executed if ISR is paused.
void isr_pause(void);

/// @brief Resumes ISRs (interrupt service routines).
///        Undoes what `isr_pause` does - interrupts will be
///        executed again. The interrupts that were "skipped"
///        because of an interrupt service routine pause will
///        not be executed.
void isr_resume(void);

#endif