#include <stdexcept>

namespace fdbg {

class InvalidId : public std::exception {
};

class Timeout : public std::exception {
};

}