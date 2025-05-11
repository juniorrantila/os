#include <hardware/interrupt.h>
#include <core/kprintf.h>
#include <core/base.h>
#include <core/string_view.h>
#include <hardware/serial.h>
#include <hardware/io.h>

typedef enum {
    Command_Count,
    Command_Exit,
    Command_Help,
    Command_Show,
    Command_Inc,
    Command_Dec,
    Command_Divide,
} Command;

KSuccess parse_command(StringView, Command*);

i32 main(i32 argc, c_string argv[])
{
    (void)argc;
    (void)argv;
    u32 value = 0;

    while (1) {
        kprintf("$ ");
        char buf[1024];
        u32 len = serial_readline(SERIAL_COM1, buf, sizeof(buf));
        StringView line = string_view_from_parts(buf, len);
        Command command = { 0 };
        if (!parse_command(line, &command).ok) {
            kprintf("unknown command: '%.*s', see 'help'\n", line.count, line.items);
            continue;
        }
        switch (command) {
        case Command_Help:
            kprintf("commands: help, exit, count, show, inc, dec, div\n");
            continue;
        case Command_Exit:
            return value;
        case Command_Count:
            kprintf("starting to count\n");
            for (u32 i = 0; i < value; i++) {
                kprintf("count: %u\n", i);
            }
            kprintf("done counting\n");
            continue;
        case Command_Show:
            kprintf("value: %u\n", value);
            continue;
        case Command_Inc:
            kprintf("value: %u\n", ++value);
            continue;
        case Command_Dec:
            kprintf("value: %u\n", --value);
            continue;
        case Command_Divide:
            kprintf("10 / %u = %u\n", value, 10 / value);
            continue;
        }
    }

    return 0;
}

KSuccess parse_command(StringView command, Command* out)
{
    const StringView help = string_view("help");
    const StringView exit = string_view("exit");
    const StringView count = string_view("count");
    const StringView show = string_view("show");
    const StringView inc = string_view("inc");
    const StringView dec = string_view("dec");
    const StringView div = string_view("div");

    if (string_view_equal(command, count))
        return *out = Command_Count, ksuccess();

    if (string_view_equal(command, exit))
        return *out = Command_Exit, ksuccess();

    if (string_view_equal(command, help))
        return *out = Command_Help, ksuccess();

    if (string_view_equal(command, show))
        return *out = Command_Show, ksuccess();

    if (string_view_equal(command, inc))
        return *out = Command_Inc, ksuccess();

    if (string_view_equal(command, dec))
        return *out = Command_Dec, ksuccess();

    if (string_view_equal(command, div))
        return *out = Command_Divide, ksuccess();

    return kfailure();
}
