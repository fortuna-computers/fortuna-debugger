#ifndef STARTUP_HH_
#define STARTUP_HH_

#include "window.hh"

class Startup : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "startup"; }
};

#endif //STARTUP_HH_
