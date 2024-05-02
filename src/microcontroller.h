#ifndef MICROCONTROLLER_H_
#define MICROCONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>

#define UART_NO_DATA ((uint16_t) -1)

void     microcontroller_init();
void     uart_send_byte(uint8_t byte);
uint16_t uart_receive_byte();

#endif //MICROCONTROLLER_H_