#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_init();

void enable_interrupts();
void disable_interrupts();

#endif
