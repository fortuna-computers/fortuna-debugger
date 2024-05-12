#ifndef CONFIGURATION_HH_
#define CONFIGURATION_HH_

#include "window.hh"

class Configuration : public Window {
public:
    using Window::Window;

    void draw() override;

    std::string name() const override { return "config"; }
};

#endif