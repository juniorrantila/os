#include "./multiboot.h"

#include <core/kprintf.h>
#include <core/string_view.h>
#include <hardware/acpi.h>
#include <hardware/interrupt.h>
#include <hardware/io.h>
#include <hardware/pci.h>
#include <hardware/serial.h>

#define MAX_ARGC 64

i32 main(i32 argc, c_string argv[]);

static i32 parse_args(StringView cmdline, i32 max_argc, c_string argv[]);

static void count_devices(void* ctx, PCIDevice const* device)
{
    (void)device;
    (*(u64*)ctx) += 1;
}

static void log_device(void* ctx, PCIDevice const* device)
{
    u64 device_index = (*(u64*)ctx)++;
    kinfo("- [%llu]", device_index);
    kinfo("  - bus: %u", device->bus);
    kinfo("  - device: %u", device->device);
    kinfo("  - function: %u", device->function);
}

__attribute__((noreturn))
void kernel_main(unsigned int multiboot_magic, multiboot_info_t* info)
{
    if (!serial_init(SERIAL_COM1)) shutdown();
    kprintf("\033[H\033[2J"); // Move top left and clear screen.
    kassert(multiboot_magic == 0x2BADB002);
    kinfo("Hello, World!");

    multiboot_dump(info);

    if (!interrupt_init().ok) kpanic("could not initialize interrupts");
    if (!acpi_init().ok) kpanic("could not initialize acpi");

    kinfo("testing PCI via manual probing...");
    if (is_pci_io_supported()) kinfo("PCI IO supported");
    else kinfo("PCI IO not supported");

    u64 devices = 0;
    pci_enumerate_devices(&devices, count_devices);
    kinfo("PCI devices: %llu", devices);
    if (devices != 0) {
        u8 device_index = 0;
        pci_enumerate_devices(&device_index, log_device);
    }

    StringView cmdline = string_view_empty();
    if (info->flags & MULTIBOOT_INFO_CMDLINE) {
        cmdline = string_view((char*)(uptr)info->cmdline);
    }

    static c_string argv[MAX_ARGC];
    i32 argc = parse_args(cmdline, MAX_ARGC, argv);
    if (argc) {
        kinfo("args(%d):", argc);
        for (int i = 0; i < argc; i++) {
            kinfo("  - [%d]: '%s'", i, argv[i]);
        }
    }

    kinfo("running main...");
    i32 code = main(argc, argv);
    kinfo("exit code: %d", code);
    kinfo("shutting down...");
    shutdown();
}

static i32 parse_args(StringView cmdline, i32 max_argc, c_string argv[])
{
    if (cmdline.count == 0) return 0;
    if (max_argc != 0) {
        kassert(argv != nullptr);
        argv[0] = &cmdline.items[0];
    }
    i32 argc = 1;
    for (u32 i = 0; i < cmdline.count; i++) {
        if (cmdline.items[i] != ' ')
            continue;
        if (argv) cmdline.items[i] = '\0';
        if (i + 1 >= cmdline.count) continue;
        if (argv) {
            kassert(argc < max_argc);
            argv[argc] = &cmdline.items[i + 1];
        }
        argc += 1;
    }

    return argc;
}
