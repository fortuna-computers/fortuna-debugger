#ifndef EMULATOR_HH_
#define EMULATOR_HH_

#include <string>
#include <thread>

extern "C" {
#include "libfdbg-server.h"
}

class Emulator {
public:
    ~Emulator();

    std::string init();
    void run_as_thread();
    void kill();

private:
    FdbgServer* server_ = nullptr;
    std::thread run_thread_;
    bool        running_ = false;

    static void run_emulator_thread(FdbgServer* server, bool& running);
};

#endif //EMULATOR_HH_
