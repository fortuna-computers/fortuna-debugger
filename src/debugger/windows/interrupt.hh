#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "window.hh"

class Interrupt : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "interrupt"; }
    bool is_debugging_window() const override { return true; }

private:
    int number_ = 0;
};

#endif