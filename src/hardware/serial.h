#pragma once
#include <core/base.h>

#define SERIAL_COM1 0x3f8

bool serial_init(u16 port);
void serial_write(u16 port, u8);
u8 serial_read(u16 port);
u32 serial_readline(u16 port, char* buf, u32 size);
