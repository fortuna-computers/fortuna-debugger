#ifndef STARTUP_HH_
#define STARTUP_HH_

#include "window.hh"

class Startup : public Window {
public:
    void draw() override;

    std::string name() const override { return "Initialization"; }
};

#endif //STARTUP_HH_
