#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"

struct idt_desc idt_descriptors[SATAN_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);

//Interrupt zero
void int_zero() {

    print("Divide by zero error\n");
}

void idt_set(int interrupt_no , void *address) {

    struct idt_desc *idt_descriptor = &idt_descriptors[interrupt_no];

    idt_descriptor->offset_1 = (uint32_t) address & 0x0000ffff;
    idt_descriptor->selector = KERNEL_CODE_SELECTOR;
    idt_descriptor->zero = 0x00;
    idt_descriptor->type_attr = 0xEE;
    idt_descriptor->offset_2 = (uint32_t) address >> 16;

}

void idt_init() {

    memset(idt_descriptors , 0 , sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    //---------------------Set your interrupts-------------------
    idt_set(0 , int_zero);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
