#include <stdexcept>

namespace fdbg {

class InvalidId : public std::runtime_error {
public:
    InvalidId() : std::runtime_error("The emulator reported an invalid id") {}
};

class Timeout : public std::runtime_error {
public:
    Timeout() : std::runtime_error("Communication timed out") {}
};

class WriteMemoryValidationError : public std::runtime_error {
public:
    explicit WriteMemoryValidationError(uint64_t first_failed_pos)
        : std::runtime_error("Memory validation error."), first_failed_pos(first_failed_pos) {}

    const uint64_t first_failed_pos;
};

}