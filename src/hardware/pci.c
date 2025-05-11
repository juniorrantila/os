#include "./pci.h"

#include "./io.h"

#include <core/base.h>
#include <core/kprintf.h>

void pci_enumerate_devices(void* context, void (*callback)(void*, PCIDevice const*))
{
    (void)context;
    (void)callback;
    kwarn_not_implemented();
}


bool is_pci_io_supported()
{
    out32(PCI_ADDRESS_PORT, 0x80000000);
    if (in32(PCI_ADDRESS_PORT) == 0x80000000) {
        return true;
    }
    return false;
}
