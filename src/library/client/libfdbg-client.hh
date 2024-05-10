#ifndef LIBFDBG_CLIENT_HH_
#define LIBFDBG_CLIENT_HH_

#include "protobuf/to-computer.pb.h"
#include "protobuf/to-debugger.pb.h"

#include "../common/common.h"

#include <optional>
#include <string>
#include <vector>

// this class is used by the debugger or the tests scripts to connect to the computer/emulator

enum class DebuggingLevel { NORMAL, DEBUG, TRACE };

class FdbgClient {
public:
    ~FdbgClient();

    void connect(std::string const& port, uint32_t baudrate);

    void set_debugging_level(DebuggingLevel debugging_level) { debugging_level_ = debugging_level; }

    void                 ack(uint32_t id);
    void                 write_memory(uint64_t pos, std::vector<uint8_t> const& data, bool validate);
    std::vector<uint8_t> read_memory(uint64_t pos, uint8_t sz, uint8_t sequences=1);

    static std::string autodetect_usb_serial_port(std::string const& vendor_id, std::string const& product_id);

private:
    int            fd_;
    DebuggingLevel debugging_level_ = DebuggingLevel::NORMAL;

    fdbg::ToDebugger send_message(fdbg::ToComputer const& msg, fdbg::ToDebugger::MessageCase expected_response, bool check_for_errors=true);

    void             send(fdbg::ToComputer const& msg);
    fdbg::ToDebugger receive(fdbg::ToDebugger::MessageCase expected_response, bool check_for_errors);
};

#endif //LIBFDBG_CLIENT_HH_
