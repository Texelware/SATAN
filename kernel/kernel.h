#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>

struct bootinfo {
    size_t page_info_table_address;  // Physical address of page info table
};

extern struct bootinfo bootinfo;

void kernel_main();

#endif
