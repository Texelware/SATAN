#include "paging.h"
#include <memory/heap/kheap.h>
#include <status.h>

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024

void enable_paging();
void paging_load_directory(size_t directory);
void paging_set_table_entry(size_t directory_address, size_t table_offset, size_t value);
size_t paging_current_page_table = 0;

size_t paging_init() {
    
    size_t kernel_address_space = paging_new_table(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(kernel_address_space);
    enable_paging();
    return kernel_address_space;
}

size_t paging_new_table(uint8_t flags)
{
    size_t *table = kmalloc(sizeof(size_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        size_t *entry = kmalloc(sizeof(size_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        table[i] = (size_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    return (size_t)table;
}

void paging_switch(size_t table)
{
    paging_load_directory(table);
    paging_current_page_table = table;
}

static void paging_get_indexes(size_t virtual_address, size_t *directory_index_out, size_t *table_index_out)
{
    *directory_index_out = ((size_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((size_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
}

int paging_set(size_t page_table, size_t virtual_address, size_t physical_address, uint8_t flags)
{
    if (!paging_is_aligned(virtual_address)) return -EINVARG;
    if (!paging_is_aligned(physical_address)) return -EINVARG;
    size_t directory_index, table_index;
    paging_get_indexes(virtual_address, &directory_index, &table_index);
    paging_set_table_entry(page_table + directory_index * sizeof(size_t), table_index * sizeof(size_t), physical_address | flags);
    return 0;
}

bool paging_is_aligned(size_t addr)
{
    return (addr % PAGING_PAGE_SIZE) == 0;
} 
