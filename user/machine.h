#ifndef MACHINE_H_
#define MACHINE_H_

#include <stdint.h>


uint16_t    machine_id();
const char* machine_name();
uint64_t    total_mappable_memory();

uint32_t    uart_baud_rate();
const char* microcontroller_vendor_id();
const char* microcontroller_product_id();

#endif //_MACHINE_H_
