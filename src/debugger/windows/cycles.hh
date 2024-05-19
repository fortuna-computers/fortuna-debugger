#ifndef CYCLES_HH_
#define CYCLES_HH_

#include "window.hh"

class Cycles : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "cycles"; }
    bool is_debugging_window() const override { return true; }
};

#endif