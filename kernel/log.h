#ifndef LOG_H
#define LOG_H

int terminal_initialize();
int kputchar(char ch);
int kputs(const char* str);
int kprintf(const char* format, ...);

#endif
