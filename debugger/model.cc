#include "model.hh"

#include "user/machine.h"

DebuggerModel::~DebuggerModel()
{
    emulator_.kill();
}

void DebuggerModel::connect_to_emulator()
{
    std::string port = emulator_.init();

    emulator_.run_as_thread();

    connect_to_serial_port(port, EMULATOR_BAUD_RATE);
}

void DebuggerModel::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    client_.set_debugging_level(DebuggingLevel::TRACE);
    client_.ack_sync(MACHINE_ID);
}

void DebuggerModel::change_memory_page(int64_t page)
{
    if (page >= memory.pages)
        page = 0;
    if (page < 0)
        page = memory.pages - 1;

    memory.data_present = false;
    memory.current_page = page;
    for (size_t i = 0; i < 4; ++i)
        client_.read_memory_async((page * 256) + (i * 64), 64);
}

void DebuggerModel::update()
{
    if (!connected_)
        return;

    std::optional<fdbg::ToDebugger> msg;
    while ((msg = client_.receive_message())) {

        switch (msg->message_case()) {
            case fdbg::ToDebugger::MessageCase::kMemoryUpdate:
                for (size_t i = 0; i < msg->memory_update().bytes().size(); ++i) {
                    int64_t idx = (int64_t) msg->memory_update().initial_pos() + i - (memory.current_page * 256);
                    if (idx >= 0 && idx < 256)
                        memory.data[idx] = msg->memory_update().bytes().at(i);
                }
                memory.data_present = true;
                break;
            case fdbg::ToDebugger::kAckResponse:
            case fdbg::ToDebugger::kWriteMemoryConfirmation:
            case fdbg::ToDebugger::MESSAGE_NOT_SET:
                break;
        }
        
    }
}

