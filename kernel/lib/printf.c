#ifndef PRINTF_H
#define PRINTF_H

#define STB_SPRINTF_DECORATE(name) name
#define STB_SPRINTF_IMPLEMENTATION 1
#define STB_SPRINTF_NOFLOAT 1

#include "stb_sprintf.h"
#include "../lib/util.h"

#define PRINT_BUFFER_SIZE    8192

int printf(const char *format, ...)
{
    va_list args;
    char buffer[PRINT_BUFFER_SIZE];
    size_t length;

    va_start(args, format);

    length = vsnprintf(buffer, PRINT_BUFFER_SIZE, format, args);
    for (size_t n=0;n<length;n++) {
        outb(0x3F8, buffer[n]);
    }
    va_end(args);

    return length;
}

#endif