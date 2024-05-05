#ifndef D_EXCEPTIONS_HH_
#define D_EXCEPTIONS_HH_

#include <stdexcept>

class DebuggerError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

#endif //EXCEPTIONS_HH_
