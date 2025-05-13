#include "./interrupt.h"

#include "./io.h"

#include <core/base.h>
#include <core/kprintf.h>
#include <core/string.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	// io_wait();
	out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	// io_wait();
	out8(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	// io_wait();
	out8(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	// io_wait();
	out8(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	// io_wait();
	out8(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	// io_wait();

	out8(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	// io_wait();
	out8(PIC2_DATA, ICW4_8086);
	// io_wait();

	// Unmask both PICs.
	out8(PIC1_DATA, 0);
	out8(PIC2_DATA, 0);
}

void pic_disable(void) {
    out8(PIC1_DATA, 0xff);
    out8(PIC2_DATA, 0xff);
}

// Note: These values are x86-64.
#define GDT_SELECTOR_CODE0 0x08
#define GDT_SELECTOR_DATA0 0x10
#define GDT_SELECTOR_DATA3 0x18
#define GDT_SELECTOR_CODE3 0x20
#define GDT_SELECTOR_TSS 0x28
#define GDT_SELECTOR_TSS_PART2 0x30

typedef struct [[gnu::packed]] {
    u16 limit;
    void* address;
} DescriptorTablePointer;

typedef enum : u8 {
  IDTEntryType_TaskGate32        = 0b0101,
  IDTEntryType_InterruptGate16   = 0b110,
  IDTEntryType_TrapGate16        = 0b111,
  IDTEntryType_InterruptGate32   = 0b1110,
  IDTEntryType_TrapGate32        = 0b1111,
} IDTEntryType;

typedef struct [[gnu::packed]] {
    u16 offset_1; // offset bits 0..15
    u16 selector; // a code segment selector in GDT or LDT

    struct {
        u8 interrupt_stack_table : 3;
        u8 zero : 5; // unused, set to 0
    };

    struct {
        u8 gate_type : 4;
        u8 storage_segment : 1;
        u8 descriptor_privilege_level : 2;
        u8 present : 1;
    } type_attr;  // type and attributes
    u16 offset_2; // offset bits 16..31
    u32 offset_3;
    u32 zeros;
} IDTEntry;
static_assert(sizeof(IDTEntry) == 2 * sizeof(u64));

static inline IDTEntry idt_entry(void* callback, u16 selector, IDTEntryType type, u8 privilege_level)
{
    return (IDTEntry){
      .offset_1 = (u16)((u64)callback & 0xFFFF),
      .selector = selector,
      .interrupt_stack_table = 0,
      .zero = 0,
      .type_attr = {
          .gate_type = (u8)type,
          .storage_segment = 0,
          .descriptor_privilege_level = (u8)(privilege_level & 0b11),
          .present = 1,
      },
      .offset_2 = (u16)((u64)callback >> 16),
      .offset_3 = (u32)(((u64)callback) >> 32),
      .zeros = 0,
    };
}

static DescriptorTablePointer s_idtr;
static IDTEntry s_idt[256];

[[gnu::interrupt]] static void interrupt_divideerror(void*, u32);
[[gnu::interrupt]] static void interrupt_debug(void*, u32);
[[gnu::interrupt]] static void interrupt_unknownerror(void*, u32);
[[gnu::interrupt]] static void interrupt_breakpoint(void*, u32);
[[gnu::interrupt]] static void interrupt_overflow(void*, u32);
[[gnu::interrupt]] static void interrupt_boundscheck(void*, u32);
[[gnu::interrupt]] static void interrupt_illegalinstruction(void*, u32);
[[gnu::interrupt]] static void interrupt_fpuexception(void*, u32);
[[gnu::interrupt]] static void interrupt_doublefault(void*, u32);
[[gnu::interrupt]] static void interrupt_coprocessorsegmentoverrun(void*, u32);
[[gnu::interrupt]] static void interrupt_invalidtss(void*, u32);
[[gnu::interrupt]] static void interrupt_segmentnotpresent(void*, u32);
[[gnu::interrupt]] static void interrupt_stackexception(void*, u32);
[[gnu::interrupt]] static void interrupt_generalprotectionfault(void*, u32);
[[gnu::interrupt]] static void interrupt_pagefault(void*, u32);
[[gnu::interrupt]] static void interrupt_unknownerror2(void*, u32);
[[gnu::interrupt]] static void interrupt_coprocessorerror(void*, u32);
[[gnu::interrupt]] static void interrupt_unhandeled(void*, u32);

static void flush_idt(void);

KSuccess interrupt_init(void)
{
    kwarn_not_implemented();

    s_idtr.address = s_idt;
    s_idtr.limit = 256 * sizeof(IDTEntry) - 1;

    for (u32 i = 0; i < 256; i++) {
        interrupt_register(i, interrupt_unhandeled);
    }

    interrupt_register(Interrupt_DivideError, interrupt_divideerror);
    interrupt_register(Interrupt_Debug, interrupt_debug);
    interrupt_register(Interrupt_UnknownError, interrupt_unknownerror);
    interrupt_register(Interrupt_Breakpoint, interrupt_breakpoint);
    interrupt_register(Interrupt_Overflow, interrupt_overflow);
    interrupt_register(Interrupt_BoundsCheck, interrupt_boundscheck);
    interrupt_register(Interrupt_IllegalInstruction, interrupt_illegalinstruction);
    interrupt_register(Interrupt_FPUException, interrupt_fpuexception);
    interrupt_register(Interrupt_DoubleFault, interrupt_doublefault);
    interrupt_register(Interrupt_CoprocessorSegmentOverrun, interrupt_coprocessorsegmentoverrun);
    interrupt_register(Interrupt_InvalidTSS, interrupt_invalidtss);
    interrupt_register(Interrupt_SegmentNotPresent, interrupt_segmentnotpresent);
    interrupt_register(Interrupt_StackException, interrupt_stackexception);
    interrupt_register(Interrupt_GeneralProtectionFault, interrupt_generalprotectionfault);
    interrupt_register(Interrupt_PageFault, interrupt_pagefault);
    interrupt_register(Interrupt_UnknownError2, interrupt_unknownerror2);
    interrupt_register(Interrupt_CoprocessorError, interrupt_coprocessorerror);

    flush_idt();
    return ksuccess();
}

void interrupt_register(u8 i, [[gnu::interrupt]] void (*handler)(void*, u32))
{
    s_idt[i] = idt_entry((void*)handler, GDT_SELECTOR_CODE0, IDTEntryType_InterruptGate32, 0);
}

static void flush_idt(void)
{
    __asm__("lidt %0" ::"m"(s_idtr));
}

[[gnu::interrupt]] static void interrupt_divideerror(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: divide by zero");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_debug(void*, u32)
{
    kpanic("interrupt: debug");
}

[[gnu::interrupt]] static void interrupt_unknownerror(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: unknown error");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_breakpoint(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: breakpoint");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_overflow(void*, u32)
{
    kpanic("interrupt: overflow");
}

[[gnu::interrupt]] static void interrupt_boundscheck(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: bounds check");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_illegalinstruction(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: illegal instruction");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_fpuexception(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: FPU exception");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_doublefault(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: double fault");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_coprocessorsegmentoverrun(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: coprocessor segment overrun");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_invalidtss(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: invalid tss");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_segmentnotpresent(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: segment not present");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_stackexception(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: stack exception");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_generalprotectionfault(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: general protection fault");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_pagefault(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: pagefault");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_unknownerror2(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: unknown error 2");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_coprocessorerror(void*, u32)
{
    interrupt_disable();
    kpanic("interrupt: coprocessor error");
    interrupt_enable();
}

[[gnu::interrupt]] static void interrupt_unhandeled(void*, u32 n)
{
    interrupt_disable();
    kpanic("interrupt: unhandled(%u)", n);
    interrupt_enable();
}
