#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024

struct page_table
{
};

inline static size_t *paging_get_directory(struct page_table *table)
{
    return (size_t*)table + sizeof(struct page_table);
}

inline static void *paging_get_entry_address(size_t entry) {
    return (void*)(entry & 0xfffff000);
}

struct page_table *paging_current_page_table = 0;
void paging_load_directory(uint32_t *directory);

struct page_table *paging_new_table(uint8_t flags)
{
    struct page_table *table = kmalloc(sizeof(struct page_table) + sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t *entry = kmalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        paging_get_directory(table)[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    return table;
}

void paging_switch(struct page_table *table)
{
    paging_load_directory(paging_get_directory(table));
    paging_current_page_table = table;
}

static int paging_get_indexes(size_t virtual_address, size_t *directory_index_out, size_t *table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVARG;
        goto out;
    }  

    *directory_index_out = ((size_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((size_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

int paging_set(struct page_table *page_table, size_t virtual_address, size_t physical_address, uint8_t flags)
{
    if (!paging_is_aligned(virtual_address))
    {
        return -EINVARG;
    }

    if (!paging_is_aligned(physical_address))
    {
        return -EINVARG;
    }

    size_t directory_index = 0;
    size_t table_index = 0;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    size_t *table = paging_get_entry_address(paging_get_directory(page_table)[directory_index]);
    table[table_index] = physical_address | flags;

    return 0;
}

bool paging_is_aligned(size_t addr)
{
    return (addr % PAGING_PAGE_SIZE) == 0;
} 
