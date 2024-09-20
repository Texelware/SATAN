#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "idt/idt.h"
#include "io/io.h"
#include <stdint.h>
#include <stddef.h>

uint16_t *video_mem = 0;
uint16_t terminal_col = 0;
uint16_t terminal_row = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {

    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c , colour);

}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}

void terminal_initialize() {

    video_mem = (uint16_t*)0xb8000;

    terminal_col = 0;
    terminal_row = 0;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {

        for(size_t x = 0 ; x < VGA_WIDTH; x++) {

            terminal_putchar(x , y , ' ' , 0);

        }

    }
}


size_t strlen(const char * str) {

    size_t len = 0;

    while(str[len]) {

        len++;
    }

    return len;

}


void print(const char * str) {

    size_t len = strlen(str);

    for(size_t i = 0; i < len; i++) {

        terminal_writechar(str[i] , 15);

    }

}

static size_t kernel_address_space = 0;

void kernel_main()
{
    terminal_initialize();
    print("Hello satan!\n");

    //Initialize the kernel heap
    kheap_init();

    //Initialize the Interrupt Descriptor Table
    idt_init();
    //Enable interrupts
    enable_interrupts();

    // Setup paging
    kernel_address_space = paging_init();
    paging_set(kernel_address_space, 0x1000, 0x8000, PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);
    paging_set(kernel_address_space, 0x2000, 0x8000, PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    char *testPtr = (char*)0x1000;

    testPtr[0] = 's';
    testPtr[1] = 'a';
    testPtr[2] = 't';
    testPtr[3] = 'a';
    testPtr[4] = 'n';
    testPtr[5] = '\n';
    testPtr[6] = '\0';

    print((char*)0x2000);
}
