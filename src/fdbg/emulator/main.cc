#include "fdbg.hh"

#include <cstdio>

#include <thread>
using namespace std::chrono_literals;

int main()
{
    uint8_t ram[64 * 1024];

    fdbg::ComputerServer server;
    server.set_debugging_level(fdbg::DebuggingLevel::TRACE);

    printf("%s\n", server.port().c_str());

    for (;;) {
        auto omsg = server.receive();

        if (omsg) {
            if (omsg->has_ack()) {
                server.send_ack_response(0x1234);
            }

            if (omsg->has_write_memory()) {
                WriteMemory const& wm = omsg->write_memory();
                for (uint64_t i = 0; i < wm.bytes().size(); ++i)
                    ram[wm.initial_addr() + i] = wm.bytes().at(i);
            }
        }

        std::this_thread::sleep_for(1ms);
    }
}