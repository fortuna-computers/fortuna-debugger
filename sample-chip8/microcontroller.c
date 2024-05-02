#include "microcontroller.h"

#include <stdio.h>
#include "pico/stdlib.h"

void microcontroller_init()
{
    stdio_init_all();
}

void uart_send_byte(uint8_t byte)
{
    putchar(byte);
}

uint16_t uart_receive_byte()
{
    return getchar_timeout_us(0);
}