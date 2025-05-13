#pragma once
#include <core/base.h>

static constexpr u16 PCI_ADDRESS_PORT = 0xcf8;

typedef struct {
    u8 bus;
    u8 device;
    u8 function;
} PCIDevice;

void pci_enumerate_devices(void* context, void (*callback)(void*, PCIDevice const*));

bool is_pci_io_supported(void);
