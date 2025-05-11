#include <core/kprintf.h>
#include <core/base.h>
#include <core/string_view.h>
#include <hardware/serial.h>
#include <hardware/io.h>

typedef enum {
    Command_Count,
    Command_Exit,
    Command_Help,
} Command;

KSuccess parse_command(StringView, Command*);

i32 main(i32 argc, c_string argv[])
{
    (void)argc;
    (void)argv;

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
            kprintf("commands: help, exit, count\n");
            continue;
        case Command_Exit:
            return 0;
        case Command_Count:
            kprintf("starting to count\n");
            for (i32 i = 0; i < 10; i++) {
                kprintf("count: %d\n", i);
            }
            kprintf("done counting\n");
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

    if (string_view_equal(command, count))
        return *out = Command_Count, ksuccess();

    if (string_view_equal(command, exit))
        return *out = Command_Exit, ksuccess();

    if (string_view_equal(command, help))
        return *out = Command_Help, ksuccess();

    return kfailure();
}
