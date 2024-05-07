#ifndef MICROCONTROLLER_H_
#define MICROCONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>

#include "libfdbg-server.h"

#define UART_NO_DATA ((uint16_t) -1)

void     microcontroller_init();
void     uart_write_byte(FdbgServer* server, uint8_t byte);
uint16_t uart_read_byte_async(FdbgServer* server);

FdbgServerEvents* event_handler();

#endif //MICROCONTROLLER_H_