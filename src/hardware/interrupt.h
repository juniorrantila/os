#pragma once
#include <core/base.h>

static inline void interrupt_disable(void)
{
    __asm__ __volatile__("cli":::"memory");
}

static inline void interrupt_enable(void)
{
    __asm__ __volatile__("sti":::"memory");
}

static inline void halt(void)
{
    __asm__ __volatile__("hlt");
}

static inline u64 cpu_flags(void)
{
    u64 flags;
    __asm__ __volatile__(
        "pushf\n"
        "pop %0\n"
        : "=rm"(flags)
        ::"memory"
    );
    return flags;
}

static inline bool are_interrupts_enabled(void)
{
    return (cpu_flags() & 0x200) != 0;
}

__attribute__((noreturn))
static inline void stop(void)
{
    interrupt_disable();
    while (1) halt();
}
