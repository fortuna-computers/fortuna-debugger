#include "machine.h"
#include "microcontroller.h"

int main()
{
    microcontroller_init();

    FdbgServer* server = fdbg_server_init(machine_id(), (FdbgServerIOCallbacks) {
            .read_byte_async = uart_read_byte_async,
            .write_byte = uart_write_byte,
    });

    FdbgServerEvents* events = event_handler();
    for (;;) {
        fdbg_server_next(server, events);
    }

}