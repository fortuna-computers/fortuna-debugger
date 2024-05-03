#include "microcontroller.h"

void microcontroller_init()
{
    // initialize the UART hardware
#error Implement this method
}

void uart_send_byte(uint8_t byte)
{
    // send a byte via UART
#error Implement this method
}

uint16_t uart_receive_byte()
{
    // asyncronously receive a byte via UART, return UART_NO_DATA if data is not available
#error Implement this method
    return UART_NO_DATA;
}