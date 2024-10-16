#include "log.h"
#include <stdint.h>
#include <string.h>

int kputs(const char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        int result = kputchar(str[i]);
        if (result != 0) return result;
    }
    return kputchar('\n');
}

int kprintf(const char* format, ...) {
    return -1;
}
