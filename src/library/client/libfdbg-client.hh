#ifndef LIBFDBG_CLIENT_HH_
#define LIBFDBG_CLIENT_HH_

#include "library/protobuf/to-computer.pb.h"
#include "library/protobuf/to-debugger.pb.h"

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

    void                            send_message(fdbg::ToComputer const& msg) const;
    std::optional<fdbg::ToDebugger> receive_message() const;
    fdbg::ToDebugger                wait_for_response(std::function<bool(fdbg::ToDebugger const& msg)> check_function) const;

    void                 ack_sync(uint32_t id) const;
    void                 write_memory_sync(uint64_t pos, std::vector<uint8_t> const& data, bool validate) const;
    void                 read_memory_async(uint64_t pos, uint8_t sz) const;
    std::vector<uint8_t> read_memory_sync(uint64_t pos, uint8_t sz) const;

    static std::string autodetect_usb_serial_port(std::string const& vendor_id, std::string const& product_id);

private:
    int fd_ = -1;
    DebuggingLevel debugging_level_ = DebuggingLevel::NORMAL;
};

#endif //LIBFDBG_CLIENT_HH_
