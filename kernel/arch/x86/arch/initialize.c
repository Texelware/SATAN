#include "lib/elf.h"
#include "multiboot2.h"
#include <kernel.h>
#include <lib/log.h>
#include <lib/memory.h>
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

    if (multiboot_info_start < kernel_top || multiboot_info_start - kernel_top > address_limit - multiboot_info_end) bootinfo.page_info_table_address = kernel_top;
    else bootinfo.page_info_table_address = multiboot_info_end;

    // {
    //     multiboot_memory_map_t *mmap;

    //     kprintf("mmap\n");

    //     for (mmap = mmap_tag->entries;
    //          (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
    //          mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap_tag->entry_size))
    //         kprintf(" base_addr = 0x%32x%32x,"
    //                 " length = 0x%32x%32x, type = 0x%32x\n",
    //                 (unsigned)(mmap->addr >> 32),
    //                 (unsigned)(mmap->addr & 0xffffffff),
    //                 (unsigned)(mmap->len >> 32),
    //                 (unsigned)(mmap->len & 0xffffffff),
    //                 (unsigned)mmap->type);
    // }
    // {
    //     Elf32_Shdr *string_table = (Elf32_Shdr *)(((uint8_t *)elf_sections_tag->sections) + elf_sections_tag->shndx * elf_sections_tag->entsize);
    //     kprintf("%32u sections, shndx: %32u\nname of the string table: %s\n", sections_tag->num, sections_tag->shndx, string_table->sh_addr + string_table->sh_name);
    //     Elf32_Shdr *sections = (Elf32_Shdr *)sections_tag->sections;
    //     for (size_t i = 0; i < sections_tag->num; i++)
    //     {
    //         kprintf("%s", sections[i].sh_name)
    //     }
    // }
    return 0;
}
