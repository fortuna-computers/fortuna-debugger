#ifndef KEYPRESS_HH_
#define KEYPRESS_HH_

#include "window.hh"

class KeyPress : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "keypress"; }

private:
    bool do_keypress();
    std::vector<uint8_t> translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super);
};

#endif