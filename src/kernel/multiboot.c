#include "./multiboot.h"

#include <core/kprintf.h>

typedef struct {
    c_string format;
    u32 count;
} ByteFormat;

static ByteFormat byte_format(u64 bytes);

void multiboot_dump(multiboot_info_t const* info)
{
    klogf("multiboot:");
    klogf("- flags: 0x%x", info->flags);

    if (info->flags & MULTIBOOT_INFO_MEMORY) {
        klogf("- mem_lower: 0x%x", info->mem_lower);
        klogf("- mem_higher: 0x%x", info->mem_higher);
    }

    if (info->flags & MULTIBOOT_INFO_BOOTDEV) {
        klogf("- boot_device: 0x%x", info->boot_device);
    }

    if (info->flags & MULTIBOOT_INFO_CMDLINE) {
        klogf("- cmdline: '%s'", (c_string)(uptr)info->cmdline);
    }

    if (info->flags & MULTIBOOT_INFO_MODS) {
        klogf("- mods_count: %u", info->mods_count);
        klogf("- mods_addr: 0x%x", info->mods_addr);
    }

    if (info->flags & MULTIBOOT_INFO_ELF_SHDR) {
        klogf("- elf_sec.num: %u", info->elf_sec.num);
        klogf("- elf_sec.size: %u", info->elf_sec.size);
        klogf("- elf_sec.addr: 0x%x", info->elf_sec.addr);
        klogf("- elf_sec.shndx: 0x%x", info->elf_sec.shndx);
        klogf("- elf_sec.shndx: 0x%x", info->elf_sec.shndx);
    }

    if (info->flags & MULTIBOOT_INFO_MEM_MAP) {
        klogf("- mmap_length: %u", info->mmap_length);
        klogf("- mmap_addr: 0x%x", info->mmap_addr);
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

        klogf("  - available:   %u%s", available_format.count, available_format.format);
        klogf("  - reserved:    %u%s", reserved_format.count, reserved_format.format);
        klogf("  - reclaimable: %u%s", reclaimable_format.count, reclaimable_format.format);
        klogf("  - nvs:         %u%s", nvs_format.count, nvs_format.format);
        klogf("  - badram:      %u%s", badram_format.count, badram_format.format);
    }

    if (info->flags & MULTIBOOT_INFO_DRIVE_INFO) {
        klogf("- drives_length: %u", info->drives_length);
        klogf("- drives_addr: %x", info->drives_addr);
    }

    if (info->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
        klogf("- config_table: 0x%x", info->config_table);
    }

    if (info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
        klogf("- boot_loader_name: '%s'", (c_string)(uptr)info->boot_loader_name);
    }

    if (info->flags & MULTIBOOT_INFO_APM_TABLE) {
        klogf("- apm_table: 0x%x", info->apm_table);
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
