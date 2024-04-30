#ifndef UI_HH_
#define UI_HH_

#include "fdbg.hh"

class UI {
public:
    explicit UI(fdbg::DebuggerClient& client);
    ~UI();

    void run();

private:
    fdbg::DebuggerClient& client_;
    struct GLFWwindow* window_ = nullptr;
};

#endif //UI_HH_
