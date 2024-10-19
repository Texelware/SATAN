#include "kernel.h"
#include <memory/heap/kheap.h>
#include <memory/paging/paging.h>
#include <idt/idt.h>
#include <lib/log.h>
#include <stddef.h>

struct bootinfo bootinfo;
static size_t kernel_address_space = 0;

void kernel_main()
{
    kprintln("Hello satan!");

    //Initialize the kernel heap
    kheap_init();

    //Initialize the Interrupt Descriptor Table
    idt_init();
    //Enable interrupts
    enable_interrupts();

    // Setup paging
    kernel_address_space = paging_init();
    paging_set(kernel_address_space, 0x1000, 0x8000, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITEABLE);
    paging_set(kernel_address_space, 0x2000, 0x8000, PAGING_FLAG_PRESENT);

    char *testPtr = (char*)0x1000;

    testPtr[0] = 's';
    testPtr[1] = 'a';
    testPtr[2] = 't';
    testPtr[3] = 'a';
    testPtr[4] = 'n';
    testPtr[5] = '\0';

    kprintln((char*)0x2000);
}
