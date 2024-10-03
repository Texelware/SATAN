#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED  0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PRESENT      0b00000001

#define PAGING_PAGE_SIZE 4096

size_t paging_init(size_t kernel_size);

size_t paging_new_table(uint8_t flags);
void paging_switch(size_t table);
extern size_t paging_current_page_table;

int paging_set(size_t page_table, size_t virtaul_address, size_t physical_address, uint8_t flags);
bool paging_is_aligned(size_t addr);

#endif
