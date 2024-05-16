#ifndef CODE_HH_
#define CODE_HH_

#include "window.hh"

class Code : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "code"; }
    bool is_debugging_window() const override { return true; }

    void draw_buttons();
    void draw_code();

private:
    int selected_file_ = 0;
};

#endif