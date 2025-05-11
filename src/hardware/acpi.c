#include "./acpi.h"

#include "./io.h"
#include "./interrupt.h"

#include <core/string.h>
#include <core/base.h>
#include <core/kprintf.h>
#include <core/string_view.h>

// FIXME: Allocate ROM as long as it's used.
typedef struct {
    PhysicalAddress address;
    u64 size;
} ROM;
static const u64 KiB = 1024;
static const ROM rom_bios = {
    .address = (PhysicalAddress){
        .ptr = 0xe0000
    },
    .size = 128 * KiB,
};

static KFound find_rsdp_in_platform_specific_memory_locations(RSDPDescriptor20Ptr*);
static KFound find_table(RSDT const* rsdt, StringView signature, u32* index);

static KFound find_chunk_starting_with(ROM, StringView signature, u32 signature_alignment, PhysicalAddress*);
static KSuccess map_ebda(ROM*);
static KSuccess map_bios(ROM*);

KSuccess acpi_init(void)
{
    RSDPDescriptor20Ptr rsdp_descriptor;
    if (!find_rsdp_in_platform_specific_memory_locations(&rsdp_descriptor).found)
        return kfail("could not find rsdp");
    RSDPDescriptor20* rsdp = physical_as_ptr(rsdp_descriptor.address);

    kinfo("RSDP(%p):", (void*)rsdp);
    kinfo("- sig: %.8s", rsdp->base.sig);
    kinfo("- checksum: %u", rsdp->base.checksum);
    kinfo("- oem_id: '%.6s'", rsdp->base.oem_id);
    kinfo("- revision: %u", rsdp->base.revision);
    kinfo("- rsdt_ptr: %x", rsdp->base.rsdt.ptr);

    RSDT* rsdt = physical32_as_ptr(rsdp->base.rsdt);
    kinfo("RSDT(%p)", (void*)rsdt);
    kinfo("- sig: %.4s", rsdt->header.sig);
    kinfo("- length: %u", rsdt->header.length);
    kinfo("- revision: %u", rsdt->header.revision);
    kinfo("- checksum: %u", rsdt->header.checksum);
    kinfo("- oem_id: '%.6s'", rsdt->header.oem_id);
    kinfo("- oem_table_id: '%.8s'", rsdt->header.oem_table_id);
    kinfo("- oem_revision: %u", rsdt->header.oem_revision);
    kinfo("- creator_id: %u", rsdt->header.creator_id);
    kinfo("- creator_revision: %u", rsdt->header.creator_revision);
    kinfo("- tables:");
    u32 end = ((rsdt->header.length - sizeof(SDTHeader)) / sizeof(u32));
    for (u32 i = 0; i < end; i++) {
        RSDT* item = physical32_as_ptr(rsdt->table[i]);
        kinfo("  - [%u]", i);
        kinfo("    - sig '%.4s'", item->header.sig);
        kinfo("    - length: %u", item->header.length);
        kinfo("    - revision: %u", item->header.revision);
        kinfo("    - checksum: %u", item->header.checksum);
        kinfo("    - oem_id: '%.6s'", item->header.oem_id);
        kinfo("    - oem_table_id: '%.8s'", item->header.oem_table_id);
        kinfo("    - oem_revision: %u", item->header.oem_revision);
        kinfo("    - creator_id: %u", item->header.creator_id);
        kinfo("    - creator_revision: %u", item->header.creator_revision);
    }

    u32 facp_index = 0;
    if (!find_table(rsdt, string_view("FACP"), &facp_index).found)
        return kfail("could not find FACP");
    FADT* fadt = physical32_as_ptr(rsdt->table[facp_index]);
    kinfo("FACP(%p):", (void*)fadt);
    kinfo("- sci_int: %u", fadt->sci_int);

    u32 hpet_index = 0;
    if (!find_table(rsdt, string_view("HPET"), &hpet_index).found)
        return kfail("could not find HPET");
    HPET* hpet = physical32_as_ptr(rsdt->table[hpet_index]);
    kinfo("HPET(%p):", (void*)hpet);
    kinfo("- hardware_revision_id: %u", hpet->hardware_revision_id);
    kinfo("- attributes: %u", hpet->attributes);
    kinfo("- pci_vendor_id: %u", hpet->pci_vendor_id);
    kinfo("- hpet_number: %u", hpet->hpet_number);
    kinfo("- mininum_clock_tick: %u", hpet->mininum_clock_tick);
    kinfo("- page_protection: %u", hpet->page_protection);

    return ksuccess();
}

static KFound find_rsdp_in_platform_specific_memory_locations(RSDPDescriptor20Ptr* out)
{
    StringView signature = string_view_from_parts("RSD PTR ", 8);

    kinfo("trying to find rsdp from ebda");
    ROM ebda;
    if (map_ebda(&ebda).ok) {
        if (find_chunk_starting_with(ebda, signature, 16, &out->address).found) {
            kinfo("found rsdp in ebda");
            return kfound();
        }
    }

    kinfo("trying to find rsdp from bios");
    ROM bios;
    if (map_bios(&bios).ok) {
        if (find_chunk_starting_with(bios, signature, 16, &out->address).found) {
            kinfo("found rsdp in bios");
            return kfound();
        }
    }

    return knotfound();
}

KSuccess map_bios(ROM* out)
{
    *out = rom_bios;
    return ksuccess();
}

KSuccess map_ebda(ROM* out)
{
    PhysicalAddress ebda_segment_ptr = physical_address(0x40e);
    PhysicalAddress ebda = physical_address(physical_read_u16(ebda_segment_ptr) << 4);
    // The EBDA size is stored in the first byte of the EBDA in 1K units
    u64 ebda_size = physical_read_u8(ebda);
    ebda_size *= 1024;
    *out = (ROM){
        .address = ebda,
        .size = ebda_size,
    };
    return ksuccess();
}

static KFound find_chunk_starting_with(ROM rom, StringView signature, u32 signature_alignment, PhysicalAddress* out)
{
    if (rom.size < signature.count)
        return knotfound();
    u64 base = rom.address.ptr;
    u64 end = rom.address.ptr + rom.size;
    for (u64 i = base; i <= end - signature.count; i += signature_alignment) {
        PhysicalAddress candidate = physical_address(i);
        if (memequal(signature.items, physical_as_ptr(candidate), signature.count)) {
            *out = candidate;
            return kfound();
        }
    }
    return knotfound();
}

static KFound find_table(RSDT const* rsdt, StringView signature, u32* index)
{
    u32 end = ((rsdt->header.length - sizeof(SDTHeader)) / sizeof(u32));
    for (u32 i = 0; i < end; i++) {
        RSDT* item = physical32_as_ptr(rsdt->table[i]);
        StringView sig = string_view_from_parts(item->header.sig, 4);
        if (string_view_equal(signature, sig)) {
            *index = i;
            return kfound();
        }
    }
    return knotfound();
}

[[noreturn]] void shutdown(void)
{
    out16(0xB004, 0x2000); // Bochs.
    out16(0x604,  0x2000); // QEMU.
    out16(0x4004, 0x3400); // VirtualBox.
    out16(0x600,  0x34);   // Cloud Hypervisor.
    stop();
}
