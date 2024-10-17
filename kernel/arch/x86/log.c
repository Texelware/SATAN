#include <lib/log.h>
#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

static uint16_t *video_mem = 0;
static uint16_t terminal_col = 0;
static uint16_t terminal_row = 0;
static char terminal_colour = 0;

static uint16_t make_char(char c, char colour)
{
    return (colour << 8) | c;
}

static void put_char_at(int x, int y, char c, char colour)
{
    video_mem[y * VGA_WIDTH + x] = make_char(c , colour);
}

static void write(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    put_char_at(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}

int kputchar(char ch)
{
    write(ch, terminal_colour);
    return 0;
}

int terminal_initialize()
{
    video_mem = (uint16_t*)0xb8000;

    terminal_col = 0;
    terminal_row = 0;
    terminal_colour = 15;

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for(size_t x = 0 ; x < VGA_WIDTH; x++)
        {
            put_char_at(x , y , ' ' , 0);
        }
    }
    return 0;
}
