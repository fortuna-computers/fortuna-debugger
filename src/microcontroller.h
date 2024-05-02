#ifndef MICROCONTROLLER_H_
#define MICROCONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>

void    microcontroller_init();
bool    uart_ready();
void    uart_send_byte(uint8_t byte);
uint8_t uart_receive_byte();

#endif //MICROCONTROLLER_H_