#include "emulator.hh"

#include "debugger/load.hh"

using namespace std::chrono_literals;

std::string Emulator::init()
{
    server_ = fdbg_server_init_pc(machine_id(), EMULATOR_BAUD_RATE);
    return fdbg_server_serial_port(server_);
}

void Emulator::run_emulator_thread(FdbgServer* server, bool& running)
{
    emulator_init();
    emulator_reset();

    FdbgServerEvents events = {
            .write_memory = [](FdbgServer* server, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed) {
                for (size_t i = 0; i < sz; ++i)
                    emulator_ram_set(pos + i, data[i]);
                return true;
            },
            .read_memory = [](FdbgServer* server, uint64_t pos, uint8_t sz, uint8_t* out_data) {
                for (size_t i = 0; i < sz; ++i)
                    out_data[i] = emulator_ram_get(pos +i);
            },
    };

    while (running) {

        if (fdbg_server_next(server, &events) != 0)
            throw std::runtime_error("server: error reading data");

        std::this_thread::sleep_for(1ms);  // TODO - use `select`
    }
}

void Emulator::run_as_thread()
{
    running_ = true;
    run_thread_ = std::thread([&]() { Emulator::run_emulator_thread(server_, running_); });
}

void Emulator::kill()
{
    if (running_) {
        running_ = false;
        run_thread_.join();
    }
}
