#include "emulator.hh"

extern "C" {
#include "machine.h"
}

using namespace std::chrono_literals;

std::string Emulator::init()
{
    server_.init();
    return server_.port();
}

void Emulator::run_as_thread()
{
    running_ = true;

    run_thread_ = std::thread([this](){

        while (running_) {
            auto omsg = server_.receive();

            if (omsg) {
                if (omsg->has_ack()) {
                    server_.send_ack_response(MACHINE_ID);
                }

                /*
                if (omsg->has_write_memory()) {
                    WriteMemory const& wm = omsg->write_memory();
                    for (uint64_t i = 0; i < wm.bytes().size(); ++i)
                        ram[wm.initial_addr() + i] = wm.bytes().at(i);
                }
                 */
            }

            std::this_thread::sleep_for(1ms);  // TODO - use `select`
        }
    });
}

void Emulator::kill()
{
    if (running_) {
        running_ = false;
        run_thread_.join();
    }
}
