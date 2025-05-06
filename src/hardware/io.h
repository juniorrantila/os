#pragma once
#include <core/base.h>

static inline u8 inb(u16 port)
{
    u8 result;
    __asm__ __volatile__(
        "xorl %%eax, %%eax\n"
        "inb %%dx, %%al\n"
        :"=a"(result)
        :"d"(port)
    );
    return result;
}

static inline void outb(u16 port, u8 data)
{
    __asm__ __volatile__(
        "outb %%al, %%dx"::"a"(data), "d"(port)
    );
}

static inline u16 inw(u16 port)
{
    u16 a = inb(port + 0);
    u16 b = inb(port + 1);
    return a << 8 | b;
}

static inline void outw(u16 port, u16 data)
{
    outb(port + 0, (data & 0x00FF) >> 0);
    outb(port + 1, (data & 0xFF00) >> 8);
}
