#pragma once
#include "./base.h"

__attribute__((format(printf, 1, 2)))
i32 kprintf(c_string fmt, ...);

__attribute__((format(printf, 4, 5)))
void kinfo_impl(c_string func, c_string file, u32 line, c_string fmt, ...);
#define kinfo(fmt, ...) kinfo_impl(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

__attribute__((format(printf, 4, 5)))
void kwarn_impl(c_string func, c_string file, u32 line, c_string fmt, ...);
#define kwarn(fmt, ...) kwarn_impl(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

__attribute__((format(printf, 4, 5), noreturn))
void kpanic_impl(c_string func, c_string file, u32 line, c_string fmt, ...);
#define kpanic(fmt, ...) kpanic_impl(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

__attribute__((format(printf, 4, 5)))
void kerror_impl(c_string func, c_string file, u32 line, char const* fmt, ...);
#define kerror(fmt, ...) kerror_impl(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define kassert(expr) do { if (!(expr)) kpanic("assert '%s' failed", #expr); } while(0)
#define kwarn_not_implemented() kwarn("not implemented")

void kputchar(char);
