#ifndef REGISTERS_HH_
#define REGISTERS_HH_

#include "window.hh"

class Registers : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "registers"; }
    bool is_debugging_window() const override { return true; }
};

#endif
