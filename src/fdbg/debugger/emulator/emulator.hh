#ifndef EMULATOR_HH_
#define EMULATOR_HH_

#include <string>
#include <thread>

#include "fdbg.hh"

class Emulator {
public:
    std::string init();
    void run_as_thread();
    void kill();

private:
    fdbg::ComputerServer server_;
    std::thread          run_thread_;
    bool                 running_ = false;
};

#endif //EMULATOR_HH_
