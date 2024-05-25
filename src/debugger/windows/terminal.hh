#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include "window.hh"

class Terminal : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "terminal"; }
    bool is_debugging_window() const override { return true; }

private:
    TerminalModel::Mode mode_ = TerminalModel::M_ANSI;
    TerminalModel::NewLine new_line_ = TerminalModel::NL_CR;
};

#endif