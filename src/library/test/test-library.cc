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

        FdbgServerEvents events = {
            .write_memory = [](FdbgServer* server, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed) {
                return true;
            },
            .read_memory = [](FdbgServer* server, uint64_t pos, uint8_t sz, uint8_t* out_data) {
                for (size_t i = 0; i < sz; ++i)
                    out_data[i] = i + 1;
            },
        };

        while (server_running) {
            if (fdbg_server_next(server, &events) != 0) {
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

        client.ack(MACHINE_ID);

        std::vector<uint8_t> data { 1, 2, 3, 4 };
        client.write_memory(0x0, data, false);
        auto data2 = client.read_memory(0x0, data.size());
        if (data != data2)
            throw std::runtime_error("Data does not match.");

        printf("Client finalized.\n");
    }

    // finalize
    server_running = false;
    t.join();
}
