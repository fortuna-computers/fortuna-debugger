#include <cstdio>

#include "protobuf/to-computer.pb.h"

#include "client/libfdbg-client.hh"

#include <thread>
using namespace std::chrono_literals;

int main()
{
    const uint16_t MACHINE_ID = 0x3bf4;

    // start and run client

    printf("Client started.\n");

    std::string port = FdbgClient::autodetect_usb_serial_port("2e8a", "000a");  // pi pico
    printf("Port: %s\n", port.c_str());

    FdbgClient client;
    client.set_debugging_level(DebuggingLevel::TRACE);
    client.connect(port, 115200);

    printf("Client connected.\n");

    client.ack(MACHINE_ID);

    std::vector<uint8_t> data { 1, 2, 3, 4 };
    client.write_memory(0, 0x0, data, false);

    auto data2 = client.read_memory(0, 0x0, data.size());
    if (data != data2)
        throw std::runtime_error("Data does not match.");

    printf("Client finalized.\n");
}
