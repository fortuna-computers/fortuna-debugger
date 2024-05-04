#include <cstdio>

extern "C" {
#include "libfdbg-server.h"
}

#include "libfdbg-client.hh"

#include <thread>
using namespace std::chrono_literals;

int main()
{
    bool server_running = true;
    const uint16_t MACHINE_ID = 0x1234;

    // start and run server

    FdbgServer* server = fdbg_server_init_pc(MACHINE_ID, EMULATOR_BAUD_RATE);
    std::string port = fdbg_server_serial_port(server);
    printf("Server started and listening in port '%s'.\n", port.c_str());

    std::this_thread::sleep_for(100ms);

    std::thread t([&server_running, &server](){

        while (server_running) {
            if (fdbg_server_next(server, nullptr) != 0) {
                fprintf(stderr, "server: error reading data\n");
                exit(1);
            }
        }

        fdbg_server_free(server);

        printf("Server finalized.\n");
    });

    std::this_thread::sleep_for(100ms);

    // start and run client

    {
        printf("Client started.\n");

        FdbgClient client;
        client.set_debugging_level(DebuggingLevel::TRACE);
        client.connect(port, EMULATOR_BAUD_RATE);

        client.ack_sync(MACHINE_ID);

        printf("Client finalized.\n");
    }

    // finalize
    server_running = false;
    t.join();
}
