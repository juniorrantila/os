#pragma once
#include "./base.h"
#include "./kprintf.h"
#include "./string.h"

typedef struct {
    char* items;
    u32 count;
} StringView;

static inline StringView string_view(mut_c_string s)
{
    kassert(s != nullptr);
    return (StringView){
        .items = s,
        .count = (u32)strlen(s),
    };
}

static inline StringView string_view_from_parts(char* items, u32 count)
{
    if (count) kassert(items != nullptr);
    return (StringView){
        .items = items,
        .count = count,
    };
}

static inline StringView string_view_empty(void)
{
    return (StringView){
        .items = 0,
        .count = 0,
    };
}

static inline bool string_view_equal(StringView a, StringView b)
{
    if (a.count != b.count) return false;
    return memequal(a.items, b.items, a.count);
}
