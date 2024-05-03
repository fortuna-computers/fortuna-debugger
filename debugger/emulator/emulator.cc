#include "emulator.hh"

extern "C" {
#include "user/machine.h"
}

using namespace std::chrono_literals;

std::string Emulator::init()
{
    server_ = fdbg_server_init_pc(MACHINE_ID, EMULATOR_BAUD_RATE);
    return fdbg_server_serial_port(server_);
}

void Emulator::run_as_thread()
{
    running_ = true;

    run_thread_ = std::thread([this](){

        while (running_) {

            if (fdbg_server_next(server_, nullptr) != 0)
                throw std::runtime_error("server: error reading data");

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
