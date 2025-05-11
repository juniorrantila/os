#include "./base.h"

typedef struct { u64 ptr; } PhysicalAddress;
static inline PhysicalAddress physical_address(u64 ptr)
{
    return (PhysicalAddress){
        .ptr = ptr,
    };
}
static inline u8 physical_read_u8(PhysicalAddress address)
{
    return *(u8*)(uptr)address.ptr;
}
static inline u16 physical_read_u16(PhysicalAddress address)
{
    return *(u16*)(uptr)address.ptr;
}
static inline void* physical_as_ptr(PhysicalAddress address)
{
    return (void*)(uptr)address.ptr;
}

typedef struct { u32 ptr; } PhysicalAddress32;
static inline PhysicalAddress32 physical_address32(u32 ptr)
{
    return (PhysicalAddress32){
        .ptr = ptr,
    };
}
static inline void* physical32_as_ptr(PhysicalAddress32 address)
{
    return (void*)(uptr)address.ptr;
}

typedef struct { u64 ptr; } PhysicalAddress64;
static inline PhysicalAddress64 physical_address64(u64 ptr)
{
    return (PhysicalAddress64){
        .ptr = ptr,
    };
}
static inline void* physical64_as_ptr(PhysicalAddress64 address)
{
    return (void*)(uptr)address.ptr;
}
