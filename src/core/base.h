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
typedef _Bool bool;
#define true  ((bool)1)
#define false ((bool)0)

#define nullptr ((void*)0)
#endif
