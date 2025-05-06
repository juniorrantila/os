#include "./kprintf.h"

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "./stb_printf.h"

#include <hardware/serial.h>
#include <hardware/acpi.h>

#define MAX_BUFSIZE 512

static void kwrite(char const* s, int size);

__attribute__((format(printf, 1, 2)))
int kprintf(c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args); 
    kassert(size >= 0);
    kwrite(buf, size);

    __builtin_va_end(args);
    return size;
}

__attribute__((format(printf, 3, 4)))
void klogf_impl(char const* file, int line, char const* fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args); 
    kassert(size >= 0);
    kprintf("[%s:%d] %.*s\n", file, line, size, buf);

    __builtin_va_end(args);
}

__attribute__((format(printf, 1, 0), noreturn))
void kpanic_impl(char const* file, int line, char const* fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args); 
    kassert(size >= 0);
    kprintf("PANIC[%s:%d] %.*s\n", file, line, size, buf);

    __builtin_va_end(args);
    shutdown();
}

void kputchar(char c)
{
    serial_write(SERIAL_COM1, c);
}

static void kwrite(char const* s, int size)
{
    for (int i = 0; i < size; i++)
        kputchar(s[i]);
}
