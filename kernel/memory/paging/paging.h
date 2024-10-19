#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct PageInfo {
    size_t uses;
};

enum
{
    PAGING_FLAG_PRESENT = 0b00000001,
    PAGING_FLAG_WRITEABLE = 0b00000010,
    PAGING_FLAG_EXECUTABLE = 0b00000100,
    PAGING_FLAG_PROTECTED = 0b00001000,
    PAGING_FLAG_DISABLE_CACHE = 0b00010000, // Might not be implemented in all systems
    PAGING_FLAGS_ALL = 0b00011111,
};

size_t paging_init();

extern size_t page_info_table_address;

size_t paging_new_table();
void paging_switch(size_t table);
extern size_t paging_current_page_table;

int paging_set(size_t page_table, size_t virtaul_address, size_t physical_address, uint8_t flags);
bool paging_is_aligned(size_t addr);

#endif
