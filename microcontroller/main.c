#include "user/machine.h"
#include "user/microcontroller.h"

#include "libfdbg-server.h"

int main()
{
    microcontroller_init();

    FdbgServer* server = fdbg_server_init(MACHINE_ID, (FdbgServerIOCallbacks) {
        .read_byte_async = uart_read_byte_async,
        .write_byte = uart_write_byte,
    });

    for (;;) {
        fdbg_server_next(server, NULL);
    }
}