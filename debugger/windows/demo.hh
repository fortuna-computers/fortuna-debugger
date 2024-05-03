#ifndef DEMO_HH_
#define DEMO_HH_

#include "window.hh"

class Demo : public Window {
public:
    using Window::Window;

    void draw() override;
    std::string name() const override { return "demo"; }
};

#endif //DEMO_HH_
