#pragma once
#include <core/base.h>
#include <core/string_view.h>

static const u16 SERIAL_COM1 = 0x3f8;

bool serial_init(u16 port);
u32 serial_write_message(u16 port, StringView message);
void serial_write(u16 port, u8);
u8 serial_read(u16 port);
u32 serial_readline(u16 port, char* buf, u32 size);

bool serial_poll_in(u16 port);
bool serial_poll_out(u16 port);
