#pragma once
#include <core/base.h>

typedef enum : u8 {
    Interrupt_DivideError               = 0,
    Interrupt_Debug                     = 1,
    Interrupt_UnknownError              = 2,
    Interrupt_Breakpoint                = 3,
    Interrupt_Overflow                  = 4,
    Interrupt_BoundsCheck               = 5,
    Interrupt_IllegalInstruction        = 6,
    Interrupt_FPUException              = 7,
    Interrupt_DoubleFault               = 8,
    Interrupt_CoprocessorSegmentOverrun = 9,
    Interrupt_InvalidTSS                = 10,
    Interrupt_SegmentNotPresent         = 11,
    Interrupt_StackException            = 12,
    Interrupt_GeneralProtectionFault    = 13,
    Interrupt_PageFault                 = 14,
    Interrupt_UnknownError2             = 15,
    Interrupt_CoprocessorError          = 16,
} Interrupt;

KSuccess interrupt_init(void);
void interrupt_register(u8 n, [[gnu::interrupt]] void(*)(void*, u32));

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

void pic_disable(void);

__attribute__((noreturn))
static inline void stop(void)
{
    interrupt_disable();
    while (1) halt();
}
