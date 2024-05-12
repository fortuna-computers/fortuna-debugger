#ifndef MAIN_MENU_HH_
#define MAIN_MENU_HH_

#include "window.hh"

class MainMenu : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "Main menu"; }

private:
    void button(std::string const& text, std::string const& window_name);
};

#endif