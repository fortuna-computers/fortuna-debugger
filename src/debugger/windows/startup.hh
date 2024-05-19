#ifndef STARTUP_HH_
#define STARTUP_HH_

#include "window.hh"

#include <memory>

#include <imgui.h>
#include "imfilebrowser.h"

class Startup : public Window {
public:
    explicit Startup(bool visible=false);

    void draw() override;

    std::string name() const override { return "startup"; }

private:
    void save_config();

    enum ConnectionType { CT_EMULATOR, CT_SERIAL } connection_type = CT_EMULATOR;
    char serial_port_[128] { 0 };
    int  baud_rate_ = 115200;
    char machine_path_[1024] { 0 };
    char emulator_path_[1024] { 0 };
    char source_file_[1024] { 0 };
    ImGui::FileBrowser file_browser_, source_browser_, emulator_browser_;
};

#endif //STARTUP_HH_
