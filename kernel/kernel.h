#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>

struct bootinfo {
    size_t ram_size;  // Total amount of RAM available
    size_t ram_start; // End of the kernel, start of free space
};

extern struct bootinfo bootinfo;

void kernel_main();

#endif
