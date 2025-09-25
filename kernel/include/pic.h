#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/// @brief Remaps the hardware interrupts from 0..16 to 32..48
void pic_remap(void);

/// @brief Notifies the PIC about the end of hardware interrupt handling
/// @param irq The hardware interrupt number (in range 0 - 16)
void pic_eoi(uint8_t irq);

#endif