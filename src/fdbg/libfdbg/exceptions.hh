#include <stdexcept>

namespace fdbg {

class InvalidId : public std::exception {
};

class Timeout : public std::exception {
};

class WriteMemoryValidationError : public std::exception {
public:
    explicit WriteMemoryValidationError(uint64_t first_failed_pos) : first_failed_pos(first_failed_pos) {}

    const uint64_t first_failed_pos;
};

}