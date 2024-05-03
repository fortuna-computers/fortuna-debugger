#include <stdio.h>

extern "C" {
#include "libfdbg-client.h"
#include "libfdbg-server.h"
}

#include <thread>
using namespace std::chrono_literals;

int main()
{
    bool server_running = true;

    // start and run server

    FdbgServer* server = fdbg_server_init_pc(EMULATOR_BAUD_RATE);
    std::string port = fdbg_server_serial_port(server);
    printf("Server listening in port '%s'.\n", port.c_str());

    std::thread t([&server_running, &server](){

        while (server_running) {
        }

        fdbg_server_free(server);
    });

    std::this_thread::sleep_for(100ms);

    // start and run client

    FdbgClient* client = fdbg_client_init_pc(port.c_str(), EMULATOR_BAUD_RATE);
    fdbg_client_set_debugging_level(client, DL_TRACE);

    fdbg_client_free(client);

    // finalize
    server_running = false;
    t.join();
}
