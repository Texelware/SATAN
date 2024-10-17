#include "kernel.h"
#include <memory/heap/kheap.h>
#include <memory/paging/paging.h>
#include <idt/idt.h>
#include <lib/log.h>
#include <stddef.h>

static size_t kernel_address_space = 0;

void kernel_main()
{
    terminal_initialize();
    kputs("Hello satan!");
    kprintf("Hey, a number: %32u, negative: %16i and hex: %64x\n", 42, -5, 0xDEADBEEF);
    // print_hex(bootinfo.ram_size);
    // print("\n");

    //Initialize the kernel heap
    kheap_init();

    //Initialize the Interrupt Descriptor Table
    idt_init();
    //Enable interrupts
    enable_interrupts();

    // Setup paging
    kernel_address_space = paging_init();
    paging_set(kernel_address_space, 0x1000, 0x8000, PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);
    paging_set(kernel_address_space, 0x2000, 0x8000, PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    char *testPtr = (char*)0x1000;

    testPtr[0] = 's';
    testPtr[1] = 'a';
    testPtr[2] = 't';
    testPtr[3] = 'a';
    testPtr[4] = 'n';
    testPtr[5] = '\0';

    kputs((char*)0x2000);
}
