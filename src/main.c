#include <core/base.h>
#include <core/string_view.h>
#include <hardware/serial.h>

i32 main(i32 argc, c_string argv[])
{
    (void)argc;
    (void)argv;

    const StringView help = string_view("help");
    const StringView exit = string_view("exit");
    const StringView count = string_view("count");

    while (1) {
        kprintf("$ ");
        char buf[1024];
        u32 len = serial_readline(SERIAL_COM1, buf, sizeof(buf));
        StringView line = string_view_from_parts(buf, len);
        if (string_view_equal(line, help)) {
            kprintf("commands: help, exit, count\n");
            continue;
        }

        if (string_view_equal(line, exit)) {
            return 0;
        }

        if (string_view_equal(line, count)) {
            kprintf("starting to count\n");
            for (int i = 0; i < 10; i++) {
                kprintf("count: %d\n", i);
            }
            kprintf("done counting\n");
            continue;
        }

        kprintf("unknown command: '%.*s', see 'help'\n", line.count, line.items);
    }

    return 0;
}
