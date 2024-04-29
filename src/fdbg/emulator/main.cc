#include "fdbg.hh"

#include <cstdio>

extern "C" {
#include "../machine.h"
}

#include <thread>
using namespace std::chrono_literals;

int main()
{
    fdbg::ComputerServer server;
    server.set_debugging_level(fdbg::DebuggingLevel::TRACE);

    for (;;) {
        auto omsg = server.receive();

        if (omsg) {
            if (omsg->has_ack()) {
                server.send_ack_response(machine_characteristics()->id);
            }

            /*
            if (omsg->has_write_memory()) {
                WriteMemory const& wm = omsg->write_memory();
                for (uint64_t i = 0; i < wm.bytes().size(); ++i)
                    ram[wm.initial_addr() + i] = wm.bytes().at(i);
            }
             */
        }

        std::this_thread::sleep_for(1ms);
    }
}