#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include "window.hh"

class Terminal : public Window {
public:
    explicit Terminal(bool visible=false);

    void draw() override;

    std::string name() const override { return "terminal"; }
    bool is_debugging_window() const override { return true; }

private:
    TerminalModel::Mode mode_;
    TerminalModel::NewLine new_line_;
};

#endif