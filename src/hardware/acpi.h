#pragma once
#include <core/base.h>
#include <core/physical_address.h>

typedef struct [[gnu::packed]] {
    char sig[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    PhysicalAddress32 rsdt;
} RSDPDescriptor;

typedef struct [[gnu::packed]] {
    RSDPDescriptor base;
    u32 length;
    PhysicalAddress64 xsdt;
    u8 extended_checksum;
    u8 reserved[3];
} RSDPDescriptor20;
typedef struct { PhysicalAddress address; } RSDPDescriptor20Ptr;

typedef struct [[gnu::packed]] {
    char sig[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} SDTHeader;

typedef struct [[gnu::packed]] {
    SDTHeader header;
    PhysicalAddress32 table[];
} RSDT;

typedef struct [[gnu::packed]] {
    SDTHeader header;
    PhysicalAddress64 table[];
} XSDT;

typedef struct [[gnu::packed]] {
    u8 address_space;
    u8 bit_width;
    u8 bit_offset;
    u8 access_size;
    u64 address;
} GenericAddressStructure;

typedef struct [[gnu::packed]] {
    SDTHeader h;
    u8 hardware_revision_id;
    u8 attributes;
    u16 pci_vendor_id;
    GenericAddressStructure event_timer_block;
    u8 hpet_number;
    u16 mininum_clock_tick;
    u8 page_protection;
} HPET;

  // https://uefi.org/specs/ACPI/6.4/05_ACPI_Software_Programming_Model/ACPI_Software_Programming_Model.html#fixed-acpi-description-table-fadt
typedef struct [[gnu::packed]] {
    SDTHeader h;
    u32 firmware_ctrl;
    PhysicalAddress32 dsdt;
    u8 reserved;
    u8 preferred_pm_profile;
    u16 sci_int;
    u32 smi_cmd;
    u8 acpi_enable_value;
    u8 acpi_disable_value;
    u8 s4bios_req;
    u8 pstate_cnt;
    u32 PM1a_EVT_BLK;
    u32 PM1b_EVT_BLK;
    u32 PM1a_CNT_BLK;
    u32 PM1b_CNT_BLK;
    u32 PM2_CNT_BLK;
    u32 PM_TMR_BLK;
    u32 GPE0_BLK;
    u32 GPE1_BLK;
    u8 PM1_EVT_LEN;
    u8 PM1_CNT_LEN;
    u8 PM2_CNT_LEN;
    u8 PM_TMR_LEN;
    u8 GPE0_BLK_LEN;
    u8 GPE1_BLK_LEN;
    u8 GPE1_BASE;
    u8 cst_cnt;
    u16 P_LVL2_LAT;
    u16 P_LVL3_LAT;
    u16 flush_size;
    u16 flush_stride;
    u8 duty_offset;
    u8 duty_width;
    u8 day_alrm;
    u8 mon_alrm;
    u8 century;
    u16 ia_pc_boot_arch_flags;
    u8 reserved2;
    u32 flags;
    GenericAddressStructure reset_reg;
    u8 reset_value;
    u16 arm_boot_arch;
    u8 fadt_minor_version;
    u64 x_firmware_ctrl;
    u64 x_dsdt;
    GenericAddressStructure x_pm1a_evt_blk;
    GenericAddressStructure x_pm1b_evt_blk;
    GenericAddressStructure x_pm1a_cnt_blk;
    GenericAddressStructure x_pm1b_cnt_blk;
    GenericAddressStructure x_pm2_cnt_blk;
    GenericAddressStructure x_pm_tmr_blk;
    GenericAddressStructure x_gpe0_blk;
    GenericAddressStructure x_gpe1_blk;
    GenericAddressStructure sleep_control;
    GenericAddressStructure sleep_status;
    u64 hypervisor_vendor_identity;
} FADT;

KSuccess acpi_init(void);

[[noreturn]] void shutdown(void);
