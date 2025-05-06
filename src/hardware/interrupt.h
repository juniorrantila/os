#pragma once

static inline void interrupt_disable(void)
{
    __asm__ __volatile__("cli");
}

static inline void interrupt_enable(void)
{
    __asm__ __volatile__("sti");
}

static inline void halt(void)
{
    __asm__ __volatile__("hlt");
}

__attribute__((noreturn))
static inline void stop(void)
{
    interrupt_disable();
    while (1) halt();
}
