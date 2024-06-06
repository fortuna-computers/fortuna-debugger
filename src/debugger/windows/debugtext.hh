#ifndef DEBUGTEXT_HH_
#define DEBUGTEXT_HH_

#include "window.hh"

class DebugText : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "debugtext"; }
    bool is_debugging_window() const override { return true; }
};

#endif