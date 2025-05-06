#include "./acpi.h"

#include "./io.h"
#include "./interrupt.h"

__attribute__((noreturn))
void shutdown(void)
{
    outw(0xB004, 0x2000); // Bochs.
    outw(0x604,  0x2000); // QEMU.
    outw(0x4004, 0x3400); // VirtualBox.
    outw(0x600,  0x34);   // Cloud Hypervisor.
    stop();
}
