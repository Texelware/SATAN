#include "idt/idt.h"
#include "kernel.h"
#include "log.h"
#include "memory/memory.h"
#include "io/io.h"

#define SATAN_IDT_ENTRIES 256
#define SATAN_TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 8

struct idt_desc
{
    uint16_t offset_1; // Offset bits 0 - 15
    uint16_t selector; // Selector thats in our GDT
    uint8_t zero; // Does nothing, unused set to zero
    uint8_t type_attr; // Descriptor type and attributes
    uint16_t offset_2; // Offset bits 16-31

} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // Size of descriptor table -1
    uint32_t base; // Base address of the start of the interrupt descriptor table
} __attribute__((packed));

struct idt_desc idt_descriptors[SATAN_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);
extern void int21h();
extern void no_interrupt();

//Interrupt zero
void int_zero()
{
    kputs("Divide by zero error");
}

void int21h_handler()
{
    kputs("keyboard pressed!");
    outb(0x20, 0x20);
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void idt_set(int interrupt_no, void *address)
{
    struct idt_desc *idt_descriptor = &idt_descriptors[interrupt_no];

    idt_descriptor->offset_1 = (uint32_t)address & 0x0000ffff;
    idt_descriptor->selector = KERNEL_CODE_SELECTOR;
    idt_descriptor->zero = 0x00;
    idt_descriptor->type_attr = 0xEE;
    idt_descriptor->offset_2 = (uint32_t)address >> 16;
}

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    //---------------------Set your interrupts-------------------
    for(int i = 0; i < SATAN_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, int_zero);
    idt_set(0x21, int21h);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
