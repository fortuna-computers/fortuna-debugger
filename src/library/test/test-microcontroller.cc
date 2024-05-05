#include <cstdio>

extern "C" {
#include "libfdbg-server.h"
}

#include "libfdbg-client.hh"

#include <thread>
using namespace std::chrono_literals;

int main()
{
    const uint16_t MACHINE_ID = 0x3b48;

    // start and run client

    printf("Client started.\n");

    std::string port = FdbgClient::autodetect_usb_serial_port("2e8a", "000a");  // pi pico
    printf("Port: %s\n", port.c_str());

    FdbgClient client;
    client.set_debugging_level(DebuggingLevel::TRACE);
    client.connect(port, 115200);

    printf("Client connected.\n");

    client.ack_sync(MACHINE_ID);

    printf("Client finalized.\n");
}
