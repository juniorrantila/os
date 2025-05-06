#pragma once
#include <core/base.h>

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY               0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV              0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE              0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS                 0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS            0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR             0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP              0x00000040

/* Is there drive info?  */
#define MULTIBOOT_INFO_DRIVE_INFO           0x00000080

/* Is there a config table?  */
#define MULTIBOOT_INFO_CONFIG_TABLE         0x00000100

/* Is there a boot loader name?  */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME     0x00000200

/* Is there a APM table?  */
#define MULTIBOOT_INFO_APM_TABLE            0x00000400

typedef struct __attribute__((packed)) {
    u32 size;
    u64 addr;
    u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE          1
#define MULTIBOOT_MEMORY_RESERVED           2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE   3
#define MULTIBOOT_MEMORY_NVS                4
#define MULTIBOOT_MEMORY_BADRAM             5
    u32 type;
} multiboot_memory_map_t;

typedef struct {
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
} multiboot_elf_section_header_table_t;

typedef struct {
    u32 flags;
    u32 mem_lower;
    u32 mem_higher;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    multiboot_elf_section_header_table_t elf_sec;
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
} multiboot_info_t;

void multiboot_dump(multiboot_info_t const*);
