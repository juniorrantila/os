#include "./serial.h"

#include "./io.h"

bool serial_init(u16 port)
{
    out8(port + 1, 0x00);    // Disable all interrupts
    out8(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out8(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(port + 1, 0x00);    //                  (hi byte)
    out8(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    out8(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    out8(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    out8(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
    out8(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (in8(port + 0) != 0xAE) {
       return false;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    out8(port + 4, 0x0F);
    return true;
}

u32 serial_write_message(u16 port, StringView message)
{
    for (u32 i = 0; i < message.count; i++)
        serial_write(port, message.items[i]);
    return message.count;
}

void serial_write(u16 port, u8 data)
{
    while (!serial_poll_out(port));
    out8(port, data);
}

u8 serial_read(u16 port)
{
    while (!serial_poll_in(port));
    return in8(port);
}

u32 serial_readline(u16 port, char* buf, u32 size)
{
    StringView move_left = string_view("\033[1D\033[0K");
    for (u32 i = 0; i < size;) {
        buf[i] = serial_read(port);
        if (buf[i] == 0x7f) {
            buf[i] = '\0';
            if (i == 0) continue;
            i -= 1;
            serial_write_message(port, move_left);
            continue;
        }
        if (buf[i] == '\r') buf[i] = '\n';
        serial_write(port, buf[i]);
        if (buf[i] == '\n') buf[i] = '\0';
        if (buf[i] == '\0') return i;
        i += 1;
    }
    return size;
}

bool serial_poll_in(u16 port)
{
    return in8(port + 5) & 1;
}

bool serial_poll_out(u16 port)
{
    return in8(port + 5) & 0x20;
}
