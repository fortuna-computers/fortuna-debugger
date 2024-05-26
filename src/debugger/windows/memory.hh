#ifndef MEMORY_HH_
#define MEMORY_HH_

#include "window.hh"

using namespace std::string_literals;

class Memory : public Window {
public:
    Memory(bool visible, uint8_t nr, std::string const& title)
    : Window(visible), window_name_("memory"s + std::to_string(nr)), title_("Memory (" + title + ")") {}

    void draw() override;

    std::string name() const override { return window_name_; }
    bool is_debugging_window() const override { return true; }

    void go_to_page_number(int64_t page);

private:
    std::string window_name_;
    std::string title_;

    void draw_memory_table() const;
    void draw_stack() const;
};

#endif //MEMORY_HH_
