#pragma once
#include "./base.h"

__attribute__((format(printf, 1, 2)))
int kprintf(c_string fmt, ...);

__attribute__((format(printf, 3, 4)))
void klogf_impl(char const* file, int line, char const* fmt, ...);
#define klogf(fmt, ...) klogf_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

__attribute__((format(printf, 3, 4), noreturn))
void kpanic_impl(char const* file, int line, char const* fmt, ...);
#define kpanic(fmt, ...) kpanic_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define kassert(expr) do { if (!(expr)) kpanic("assert '%s' failed", #expr); } while(0)

void kputchar(char);
