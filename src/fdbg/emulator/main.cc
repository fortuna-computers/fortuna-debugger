#include "fdbg.hh"

#include <cstdio>

#include <thread>
using namespace std::chrono_literals;

int main()
{
    fdbg::ComputerServer server;
    server.set_debugging_level(fdbg::DebuggingLevel::TRACE);

    printf("%s\n", server.port().c_str());

    for (;;) {
        auto omsg = server.receive();

        if (omsg) {
            if (omsg->has_ack()) {
                server.send_ack_response(0x1234);
            }
        }

        std::this_thread::sleep_for(1ms);
    }
}