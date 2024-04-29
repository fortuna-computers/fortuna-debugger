#ifndef SERIAL_HH_
#define SERIAL_HH_

#include <cstdint>

#include <optional>

namespace fdbg {

enum class DebuggingLevel { NORMAL, DEBUG, TRACE };

class Serial {
public:
    virtual ~Serial();

    void set_debugging_level(DebuggingLevel debugging_level) { debugging_level_ = debugging_level; }

protected:
    Serial() = default;

    void configure_terminal_settings(uint32_t baud);

    template <typename T> void             send_message(T const& msg) const;
    template <typename T> std::optional<T> receive_message() const;

    int fd_ = -1;
    DebuggingLevel debugging_level_ = DebuggingLevel::NORMAL;
};

}

#include "serial.inl"

#endif //SERIAL_HH_
