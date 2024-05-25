#ifndef KEYPRESS_HH_
#define KEYPRESS_HH_

#include "window.hh"

class KeyPress : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "keypress"; }

    std::optional<std::string> key() const { return key_; }

private:
    std::optional<std::string> key_ {};

    bool do_keypress();
};

#endif