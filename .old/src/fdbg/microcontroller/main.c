#include "../../microcontroller.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "to-computer.pb.h"
#include "to-debugger.pb.h"

static uint8_t uart_receive_block()  // TODO - add timeout?
{
    uint16_t r;
    while ((r = uart_receive_byte()) == UART_NO_DATA);
    return r;
}

static bool read_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    size_t bytes_to_read = (size_t) stream->state;

    for (size_t i = 0; i < count; ++i)
        buf[i] = uart_receive_block();

    stream->bytes_left -= count;

    return stream->bytes_left == 0;
}

int main()
{
    uint8_t buffer[1024];

    microcontroller_init();

    for (;;) {
        uint8_t szi[2];
        szi[0] = uart_receive_block();
        szi[1] = uart_receive_block();
        uint16_t sz = ((uint16_t) szi[1]) << 8 | szi[0];

    }
    ToComputer msg = ToComputer_init_zero;

    pb_istream_t stream = { &read_callback, NULL, sizeof buffer };

    /*
    for (;;) {
        uint16_t r = uart_receive_byte();
        if (r != UART_NO_DATA) {
            uint8_t c = (uint8_t) r;

        }
    }
    */


}