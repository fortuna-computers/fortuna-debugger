#ifndef LIBFDBG_CLIENT_HH_
#define LIBFDBG_CLIENT_HH_

#include "common/common.h"
#include "machine.hh"
#include "debuginfo.hh"
#include "protobuf/to-debugger.pb.h"
#include "protobuf/to-computer.pb.h"

#include <optional>
#include <span>
#include <string>
#include <vector>

// this class is used by the debugger or the tests scripts to connect to the computer/emulator

enum class DebuggingLevel { NORMAL, DEBUG_, TRACE };

class FdbgClient {
public:
    ~FdbgClient();

    // initialization
    void load_user_definition(std::string const& filename);
    void connect(std::string const& port, uint32_t baudrate);

    // lower-level calls to server
    void                 ack(uint32_t id);
    fdbg::ComputerStatus reset();
    void                 write_memory(uint64_t pos, std::span<const uint8_t> const& data, bool validate=false);
    std::vector<uint8_t> read_memory(uint64_t pos, uint8_t sz, uint8_t sequences=1);
    fdbg::ComputerStatus step(bool full);

    // higher-level calls to server
    struct Upload { size_t next = 0; };
    bool write_memory_step(uint64_t pos, std::span<const uint8_t> const& data, Upload& upload, bool validate=false);

    void write_memory_full(uint64_t pos, std::span<const uint8_t> const& data, bool validate=false);
    void compile_and_write_memory(std::string const& source_filename, bool validate=false);

    // properties
    void set_debugging_level(DebuggingLevel debugging_level) { debugging_level_ = debugging_level; }
    Machine const& machine() const { return machine_; }

    // static methods
    static std::string autodetect_usb_serial_port(std::string const& vendor_id, std::string const& product_id);
    static std::string start_emulator(std::string const& path);

private:
    Machine           machine_;
    int               fd_;
    DebuggingLevel    debugging_level_ = DebuggingLevel::NORMAL;

    fdbg::ToDebugger send_message(fdbg::ToComputer const& msg, bool check_for_errors=true);

    void             send(fdbg::ToComputer const& msg);
    fdbg::ToDebugger receive(bool check_for_errors);
};

#endif //LIBFDBG_CLIENT_HH_
