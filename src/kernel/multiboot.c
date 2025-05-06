#include "./multiboot.h"

#include <core/kprintf.h>

typedef struct {
    c_string format;
    u32 count;
} ByteFormat;

static ByteFormat byte_format(u64 bytes);

void multiboot_dump(multiboot_info_t const* info)
{
    kinfo("multiboot:");
    kinfo("- flags: 0x%x", info->flags);

    if (info->flags & MULTIBOOT_INFO_MEMORY) {
        kinfo("- mem_lower: 0x%x", info->mem_lower);
        kinfo("- mem_higher: 0x%x", info->mem_higher);
    }

    if (info->flags & MULTIBOOT_INFO_BOOTDEV) {
        kinfo("- boot_device: 0x%x", info->boot_device);
    }

    if (info->flags & MULTIBOOT_INFO_CMDLINE) {
        kinfo("- cmdline: '%s'", (c_string)(uptr)info->cmdline);
    }

    if (info->flags & MULTIBOOT_INFO_MODS) {
        kinfo("- mods_count: %u", info->mods_count);
        kinfo("- mods_addr: 0x%x", info->mods_addr);
    }

    if (info->flags & MULTIBOOT_INFO_ELF_SHDR) {
        kinfo("- elf_sec.num: %u", info->elf_sec.num);
        kinfo("- elf_sec.size: %u", info->elf_sec.size);
        kinfo("- elf_sec.addr: 0x%x", info->elf_sec.addr);
        kinfo("- elf_sec.shndx: 0x%x", info->elf_sec.shndx);
        kinfo("- elf_sec.shndx: 0x%x", info->elf_sec.shndx);
    }

    if (info->flags & MULTIBOOT_INFO_MEM_MAP) {
        kinfo("- mmap_length: %u", info->mmap_length);
        kinfo("- mmap_addr: 0x%x", info->mmap_addr);
        multiboot_memory_map_t* maps = (multiboot_memory_map_t*)(uptr)info->mmap_addr;

        u64 available = 0;
        u64 reserved = 0;
        u64 reclaimable = 0;
        u64 nvs = 0;
        u64 badram = 0;
        for (u32 i = 0; i < info->mmap_length - 1; i++) {
            multiboot_memory_map_t* map = &maps[i];
            switch (map->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                available += map->len;
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                reserved += map->len;
                break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                reclaimable += map->len;
                break;
            case MULTIBOOT_MEMORY_NVS:
                nvs += map->len;
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                badram += map->len;
                break;
            }
        }
        ByteFormat available_format = byte_format(available);
        ByteFormat reserved_format = byte_format(reserved);
        ByteFormat reclaimable_format = byte_format(reclaimable);
        ByteFormat nvs_format = byte_format(nvs);
        ByteFormat badram_format = byte_format(badram);

        kinfo("  - available:   %u%s", available_format.count, available_format.format);
        kinfo("  - reserved:    %u%s", reserved_format.count, reserved_format.format);
        kinfo("  - reclaimable: %u%s", reclaimable_format.count, reclaimable_format.format);
        kinfo("  - nvs:         %u%s", nvs_format.count, nvs_format.format);
        kinfo("  - badram:      %u%s", badram_format.count, badram_format.format);
    }

    if (info->flags & MULTIBOOT_INFO_DRIVE_INFO) {
        kinfo("- drives_length: %u", info->drives_length);
        kinfo("- drives_addr: %x", info->drives_addr);
    }

    if (info->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
        kinfo("- config_table: 0x%x", info->config_table);
    }

    if (info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
        kinfo("- boot_loader_name: '%s'", (c_string)(uptr)info->boot_loader_name);
    }

    if (info->flags & MULTIBOOT_INFO_APM_TABLE) {
        kinfo("- apm_table: 0x%x", info->apm_table);
    }
}

static ByteFormat byte_format(u64 bytes)
{
    c_string format = "B";
    if (bytes >= 1024) {
        bytes /= 1024;
        format = "KiB";
    }
    if (bytes >= 1024) {
        bytes /= 1024;
        format = "MiB";
    }
    if (bytes >= 1024) {
        bytes /= 1024;
        format = "GiB";
    }

    return (ByteFormat) {
        .format = format,
        .count = bytes,
    };
}
