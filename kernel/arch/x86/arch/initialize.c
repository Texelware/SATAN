#include "lib/elf.h"
#include "multiboot2.h"
#include "paging/paging_config.h"
#include <kernel.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <memory/paging/paging.h>
#include <status.h>
#include <stdint.h>

void terminal_initialize();

struct multiboot_info
{
    uint32_t total_size;
    uint32_t reserved;
    struct multiboot_tag tags[0];
};

int initialize(uint32_t magic, struct multiboot_info *multiboot_info)
{
    terminal_initialize();

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    {
        kprintf("Invalid magic number: 0x%32x\n", magic);
        return -EINVARG;
    }

    if ((size_t)multiboot_info & 7)
    {
        kprintf("Unaligned mbi: %p\n", (size_t)multiboot_info);
        return -EINVARG;
    }

    struct multiboot_tag_mmap *mmap_tag = 0;
    struct multiboot_tag_elf_sections *elf_sections_tag = 0;
    size_t address_limit = 0;

    for (struct multiboot_tag *tag = &multiboot_info->tags[0];
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            address_limit = (((struct multiboot_tag_basic_meminfo *)tag)->mem_upper + 1024) * 1024;
            break;
        case MULTIBOOT_TAG_TYPE_MMAP:
            mmap_tag = (struct multiboot_tag_mmap *)tag;
            break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            elf_sections_tag = (struct multiboot_tag_elf_sections *)tag;
            break;
        }
    }

    if (!address_limit)
    {
        kprintln("Basic memory info is invalid or not provided!");
        return -EINVARG;
    }
    if (!mmap_tag)
    {
        kprintln("Memory map is invalid or not provided!");
        return -EINVARG;
    }
    if (!elf_sections_tag)
    {
        kprintln("Elf sections tag is invalid or not provided!");
        return -EINVARG;
    }

    size_t kernel_top = 0;
    for (Elf32_Shdr *section = (Elf32_Shdr *)elf_sections_tag->sections;
         (uint8_t *)section < (uint8_t *)elf_sections_tag + elf_sections_tag->size;
         section = (Elf32_Shdr *)((uint8_t *)section + elf_sections_tag->entsize))
    {
        size_t section_top = (section->sh_addr + section->sh_size);
        if (section_top > kernel_top) kernel_top = section_top;
    }

    size_t multiboot_info_start = (size_t)multiboot_info;
    size_t multiboot_info_end = multiboot_info_start + multiboot_info->total_size;
    kprintf("Kernel top: %32uKiB\n", kernel_top / 1024);
    kprintf("Multiboot info structure start: %32uKiB\n", (size_t)multiboot_info / 1024);
    kprintf("Multiboot info structure end: %32uKiB\n", ((size_t)multiboot_info + multiboot_info->total_size) / 1024);
    kprintf("Total memory size: %32uMiB\n", address_limit / 1024 / 1024);

    if (multiboot_info_start < kernel_top || multiboot_info_start - kernel_top > address_limit - multiboot_info_end) page_info_table_address = kernel_top;
    else page_info_table_address = multiboot_info_end;
    bootinfo.page_count = address_limit / PAGE_SIZE;
    if (page_info_table_address + bootinfo.page_count * sizeof(struct PageInfo) > address_limit)
    {
        kprintln("Can't create page info table, not enough RAM!");
        return -ENOMEM;
    }
    struct PageInfo *page_info_table = (struct PageInfo *)page_info_table_address;
    for (size_t i = 0; i < bootinfo.page_count; i++)
    {
        page_info_table[i].uses = 1;
    }

    {
        for (multiboot_memory_map_t *mmap = mmap_tag->entries;
             (uint8_t *)mmap < (uint8_t *)mmap_tag + mmap_tag->size;
             mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap_tag->entry_size))
        {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                size_t lower_page = mmap->addr / PAGE_SIZE + (mmap->addr % PAGE_SIZE != 0);
                size_t upper_page = (mmap->addr + mmap->len) / PAGE_SIZE;
                for (size_t i = lower_page; i < upper_page; i++)
                    page_info_table[i].uses = 0;
            }
        }
    }
    {
        // Elf32_Shdr *string_table = (Elf32_Shdr *)(((uint8_t *)elf_sections_tag->sections) + elf_sections_tag->shndx * elf_sections_tag->entsize);
        for (size_t i = 0; i < elf_sections_tag->num; i++)
        {
            Elf32_Shdr *section = (Elf32_Shdr *)(((uint8_t *)elf_sections_tag->sections) + i * elf_sections_tag->entsize);
            size_t upper_addr = section->sh_addr + section->sh_size;
            size_t lower_page = section->sh_addr / PAGE_SIZE;
            size_t upper_page = upper_addr / PAGE_SIZE + (upper_addr % PAGE_SIZE != 0);
            for (size_t i = lower_page; i < upper_page; i++)
            {
                if (i >= bootinfo.page_count)
                {
                    kprintln("Kernel location is outside of the memory!");
                    return -EINVARG;
                }
                page_info_table[i].uses = 1;
            }
        }
    }
    {
        size_t upper_addr = page_info_table_address + bootinfo.page_count * sizeof(struct PageInfo);
        size_t lower_page = page_info_table_address / PAGE_SIZE;
        size_t upper_page = upper_addr / PAGE_SIZE + (upper_addr % PAGE_SIZE != 0);
        for (size_t i = lower_page; i < upper_page; i++)
        {
            if (i >= bootinfo.page_count)
            {
                kprintln("Page info table is outside of the memory!");
                return -ENOMEM;
            }
            page_info_table[i].uses = 1;
        }
    }
    return 0;
}
