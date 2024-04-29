#ifndef DEMO_HH_
#define DEMO_HH_

#include "window.hh"

class Demo : public Window {
public:
    explicit Demo(fdbg::DebuggerClient& client) : Window(client) {}

    void draw() override;
    std::string name() const override { return "demo_window"; }
};

#endif //DEMO_HH_
