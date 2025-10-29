#ifndef IDT_H
#define IDT_H

void idt_init(void);

void isr_pause(void);
void isr_resume(void);

#endif