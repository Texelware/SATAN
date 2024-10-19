#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>

struct bootinfo {
    size_t page_count;
};

extern struct bootinfo bootinfo;

void kernel_main();

#endif
