#ifndef _MACHINE_H_
#define _MACHINE_H_

#include <stdint.h>

#error Define the values below

#define MACHINE_ID 0x0000       // Id that uniquely identifies the machine. This is used by the debugger
                                //    to check if the debugger is connected to the right device.
#define TOTAL_MAPPABLE_MEMORY   // Total memory that is theoretically accessible by the CPU. Memory access
                                //    to these addresses should never fail.
#define TOTAL_ACTUAL_MEMORY     // Total RAM memory. Used by the emulator to allocate the memory on the PC.

#define UART_BAUD_RATE 115200   // Baud rate of the microcontroller

// These are optional variables that will help the debugger to automatically find the serial port if connected via USB.
// Always use 4-digit hex (ex. "080a").
#define MICROCONTROLLER_VENDOR_ID  "0000"
#define MICROCONTROLLER_PRODUCT_ID "0000"

#endif //_MACHINE_H_