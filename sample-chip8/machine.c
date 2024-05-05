#include <stdint.h>

uint16_t machine_id()
{
    return 0x3bf4;
}

const char* machine_name()
{
    return "CHIP-8";
}

uint64_t total_mappable_memory()
{
    return 4 * 1024;
}

uint32_t uart_baud_rate()
{
    return 115200;
}

const char* microcontroller_vendor_id()
{
    return "2e8a";
}

const char* microcontroller_product_id()
{
    return "000a";
}