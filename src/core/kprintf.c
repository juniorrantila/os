#include "./kprintf.h"

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "./stb_printf.h"

#include <arch/i386/serial.h>
#include <arch/i386/acpi.h>

#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"
#define CYAN   "\033[1;36m"
#define NORMAL "\033[0;0m"

#define MAX_BUFSIZE 512

static void kwrite(char const* s, int size);

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

void kinfo_impl(c_string func, c_string file, u32 line, c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args);
    kassert(size >= 0);
    kprintf(BLUE "INFO" NORMAL "[%s:%d][%s] %.*s\n", file, line, func, size, buf);

    __builtin_va_end(args);
}

void kwarn_impl(c_string func, c_string file, u32 line, c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args);
    kassert(size >= 0);
    kprintf(YELLOW "WARN" NORMAL "[%s:%d][%s] %.*s\n", file, line, func, size, buf);

    __builtin_va_end(args);
}

[[noreturn]] void kpanic_impl(c_string func, c_string file, u32 line, c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args);
    kassert(size >= 0);

    kprintf(RED "PANIC" NORMAL "[%s:%d][%s] %.*s\n", file, line, func, size, buf);

    __builtin_va_end(args);
    shutdown();
}

void kerror_impl(c_string func, c_string file, u32 line, c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args);
    kassert(size >= 0);
    kprintf(RED "ERROR" NORMAL "[%s:%d][%s] %.*s\n", file, line, func, size, buf);

    __builtin_va_end(args);
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

KSuccess kfail_impl(c_string func, c_string file, u32 line, c_string fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    static char buf[MAX_BUFSIZE];
    int size = stbsp_vsnprintf(buf, MAX_BUFSIZE, fmt, args);
    kassert(size >= 0);
    kprintf(RED "ERROR" NORMAL "[%s:%d][%s] %.*s\n", file, line, func, size, buf);

    __builtin_va_end(args);
    return kfailure();
}
