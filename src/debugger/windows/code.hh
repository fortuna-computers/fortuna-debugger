#ifndef CODE_HH_
#define CODE_HH_

#include "window.hh"

#include <optional>

class Code : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "code"; }
    bool is_debugging_window() const override { return true; }

    void draw_buttons();
    void draw_code();
    void draw_footer();

private:
    int selected_file_ = 0;
    int selected_symbol_ = 0;
    std::optional<uint64_t> scroll_to_addr_in_next_frame_;
};

#endif