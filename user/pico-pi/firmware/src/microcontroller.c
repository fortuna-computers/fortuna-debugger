#include "pico/stdlib.h"

#include "user/microcontroller.h"

#include <stdio.h>

void microcontroller_init()
{
    stdio_init_all();
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