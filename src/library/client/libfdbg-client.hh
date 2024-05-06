#ifndef LIBFDBG_CLIENT_HH_
#define LIBFDBG_CLIENT_HH_

#include "library/protobuf/to-computer.pb.h"
#include "library/protobuf/to-debugger.pb.h"

#include "../common/common.h"
#include "communicationqueue.hh"

#include <optional>
#include <string>
#include <vector>

// this class is used by the debugger or the tests scripts to connect to the computer/emulator

class FdbgClient {
public:
    ~FdbgClient();

    void connect(std::string const& port, uint32_t baudrate) { comm_queue_.connect(port, baudrate); }

    void set_debugging_level(DebuggingLevel debugging_level) { comm_queue_.set_debugging_level(debugging_level); }

    std::optional<fdbg::ToDebugger> receive_message();

    void                 ack_sync(uint32_t id);
    void                 write_memory_sync(uint64_t pos, std::vector<uint8_t> const& data, bool validate);
    void                 read_memory_async(uint64_t pos, uint8_t sz);
    std::vector<uint8_t> read_memory_sync(uint64_t pos, uint8_t sz);

    static std::string autodetect_usb_serial_port(std::string const& vendor_id, std::string const& product_id);

private:
    CommunicationQueue comm_queue_;

    fdbg::ToDebugger wait_for_message_of_type(fdbg::ToDebugger::MessageCase messageType);
};

#endif //LIBFDBG_CLIENT_HH_
