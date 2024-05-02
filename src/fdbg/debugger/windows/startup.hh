#ifndef STARTUP_HH_
#define STARTUP_HH_

#include "window.hh"

#include "machine.h"

class Startup : public Window {
public:
    explicit Startup(class UIInterface& ui, bool visible=false);

    void draw() override;

    std::string name() const override { return "startup"; }

private:
    void save_config();

    enum ConnectionType { CT_EMULATOR, CT_SERIAL } connection_type = CT_EMULATOR;
    bool real_hardware_ = false;
    char serial_port_[128] { 0 };
    int baud_rate_ = UART_BAUD_RATE;
};

#endif //STARTUP_HH_
