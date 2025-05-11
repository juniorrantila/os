#pragma once
#include <core/base.h>

static inline u8 in8(u16 port)
{
  u8 value;
  __asm__ __volatile__(
    "inb %1, %0"
    : "=a"(value)
    : "Nd"(port)
);
  return value;
}

static inline u16 in16(u16 port)
{
  u16 value;
  __asm__ __volatile__(
      "inw %1, %0"
      : "=a"(value)
      : "Nd"(port)
  );
  return value;
}

static inline u32 in32(u16 port)
{
  u32 value;
  __asm__ __volatile__(
      "inl %1, %0"
      : "=a"(value)
      : "Nd"(port)
  );
  return value;
}

static inline void out8(u16 port, u8 value)
{
    __asm__ __volatile__("outb %0, %1" ::"a"(value), "Nd"(port));
}

static inline void out16(u16 port, u16 value)
{
    __asm__ __volatile__("outw %0, %1" ::"a"(value), "Nd"(port));
}

static inline void out32(u16 port, u32 value)
{
    __asm__ __volatile__("outl %0, %1" ::"a"(value), "Nd"(port));
}

static inline void delay(u64 microseconds)
{
    for (u64 i = 0; i < microseconds; i++)
        in8(0x80);
}
