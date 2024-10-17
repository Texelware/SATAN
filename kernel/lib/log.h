#ifndef LOG_H
#define LOG_H

int terminal_initialize();
int kputchar(char ch);
int kprint(const char* str);
int kprintln(const char* str);
int kprintf(const char* format, ...);

#endif
