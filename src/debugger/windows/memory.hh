#ifndef MEMORY_HH_
#define MEMORY_HH_

#include "window.hh"

class Memory : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "memory"; }

    void go_to_page_number(int64_t page);

private:
    void draw_memory_table() const;
    void draw_stack() const;
};

#endif //MEMORY_HH_
