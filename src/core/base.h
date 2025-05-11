#pragma once

typedef __INT8_TYPE__  i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;
typedef __INT64_TYPE__ i64;

typedef __UINT8_TYPE__  u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

typedef __UINTPTR_TYPE__ uptr;
typedef __INTPTR_TYPE__ iptr;

typedef __SIZE_TYPE__ usize;
typedef __INTPTR_TYPE__ isize;

typedef char const* c_string;

#ifndef __cplusplus
#if defined(__STDC_VERSION__) && __STDC_VERSION__ <= 201710L
typedef _Bool bool;
#define true  ((bool)1)
#define false ((bool)0)
#define nullptr ((void*)0)
#endif
#endif

typedef struct [[nodiscard]] { bool ok; } KSuccess;
static inline KSuccess ksuccess() { return (KSuccess){true}; }
static inline KSuccess kfailure() { return (KSuccess){false}; }

typedef struct [[nodiscard]] { bool found; } KFound;
static inline KFound kfound() { return (KFound){true}; }
static inline KFound knotfound() { return (KFound){false}; }

__attribute__((format(printf, 4, 5)))
KSuccess kfail_impl(c_string func, c_string file, u32 line, c_string fmt, ...);
#define kfail(fmt, ...) kfail_impl(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
