#include "kernel.h"
#include "memory/memory.h"
#include <stdint.h>

struct bootinfo bootinfo;

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_basic_memory_info {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
};

struct multiboot_info {
    uint32_t total_size;
    uint32_t reserved;
    struct multiboot_tag tags[0];
};

void load_bootinfo(struct multiboot_info *multiboot_info) {
    memset(&bootinfo, 0, sizeof(bootinfo));
    struct multiboot_tag *tag = &multiboot_info->tags[0];
    while (tag->type != 0)
    {
        if (tag->type == 4)
        {
            bootinfo.ram_size = (((struct multiboot_tag_basic_memory_info*)tag)->mem_upper + 1024) * 1024;
        }
        size_t tag_addr = (size_t)tag + tag->size;
        if (tag_addr % 8 != 0) tag_addr += 8 - tag_addr % 8;
        tag = (struct multiboot_tag*)tag_addr;
    }
}
