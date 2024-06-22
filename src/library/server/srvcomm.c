#include "srvcomm.h"
#include "pb_decode.h"
#include "pb_encode.h"

static uint8_t comm_read_byte_sync(FdbgServer* server)
{
    for (;;) {
        uint16_t r = server->io_callbacks.read_byte_async(server);
        if (r <= 0xff)
            return (uint8_t) r;
    }
}

bool comm_receive_next_message(FdbgServer* server, fdbg_ToComputer* msg, bool* error)
{
    *error = false;

    // get size
    uint16_t sz = server->io_callbacks.read_byte_async(server);
    if (sz == SERIAL_ERROR) {
        *error = true;
        return false;
    } else if (sz == SERIAL_NO_DATA) {
        return false;
    }

    if (sz & (1 << 7))
        sz = (comm_read_byte_sync(server) << 7) | (sz & 0x7f);

    // read message
    uint8_t buf[sz];
    for (size_t i = 0; i < sz; ++i)
        buf[i] = comm_read_byte_sync(server);

    // parse message
    pb_istream_t stream = pb_istream_from_buffer(buf, sz);
    return pb_decode(&stream, fdbg_ToComputer_fields, msg);
}

int comm_send_message(FdbgServer* server, fdbg_ToDebugger* msg)
{
    uint8_t buf[MAX_MESSAGE_SZ];

    pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof buf);
    bool status = pb_encode(&stream, fdbg_ToDebugger_fields, msg);
    if (status) {
        if (stream.bytes_written <= 0x7f) {
            server->io_callbacks.write_byte(server, stream.bytes_written);
        } else {
            server->io_callbacks.write_byte(server, (stream.bytes_written & 0x7f) | 0x80);
            server->io_callbacks.write_byte(server, stream.bytes_written >> 7);
        }

        for (size_t i = 0; i < stream.bytes_written; ++i)
            server->io_callbacks.write_byte(server, buf[i]);

        return 0;
    } else {
        return -1;
    }
}

