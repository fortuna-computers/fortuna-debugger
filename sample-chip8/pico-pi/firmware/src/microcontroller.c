#include "pico/stdlib.h"

#include "microcontroller.h"

#include <stdio.h>

#include "emulator.h"

void microcontroller_init()
{
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);

    emulator_init();
    emulator_reset();
}

void uart_write_byte(FdbgServer* server, uint8_t byte)
{
    (void) server;
    putchar(byte);
}

uint16_t uart_read_byte_async(FdbgServer* server)
{
    (void) server;
    return getchar_timeout_us(0);
}

//
// event handler
//

static bool write_memory(FdbgServer* server, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    for (size_t i = 0; i < sz; ++i)
        emulator_ram_set(pos + i, data[i]);
    return true;
}

static void read_memory(FdbgServer* server, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
    for (size_t i = 0; i < sz; ++i)
        out_data[i] = emulator_ram_get(pos + i);
}

FdbgServerEvents* event_handler()
{
    static FdbgServerEvents events = {
            .read_memory = read_memory,
            .write_memory = write_memory,
    };
    return &events;
}