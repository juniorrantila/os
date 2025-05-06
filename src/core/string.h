#pragma once
#include "./base.h"
#include "./kprintf.h"

static inline void* memset(void* b, int c, usize len)
{
    u8* bytes = (u8*)b;
    for (usize i = 0; i < len; i++) {
        bytes[i] = c;
    }
    return b;
}

static inline bool memequal(void const* a, void const* b, u64 len)
{
    if (len == 0) return true;
    kassert(a != nullptr);
    kassert(b != nullptr);
    u8 const* ap = (u8 const*)a;
    u8 const* bp = (u8 const*)b;
    for (u64 i = 0; i < len; i++) {
        if (ap[i] != bp[i]) return false;
    }
    return true;
}

static inline int strlen(c_string s)
{
    kassert(s != nullptr);
    int len = 0;
    while(s[len]) len += 1;
    return len;
}
