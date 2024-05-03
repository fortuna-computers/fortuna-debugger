#include "startup.hh"

#include <imgui.h>

#include "../ui.hh"

extern "C" {
#include "machine.h"
}

#include "../exceptions/exceptions.hh"

Startup::Startup(UIInterface& ui, bool visible)
    : Window(ui, visible)
{
    if (ui.config().get("Connection type") == "hardware")
        connection_type = CT_SERIAL;

    strncpy(serial_port_, ui.config().get("Serial port").c_str(), IM_ARRAYSIZE(serial_port_));
}

void Startup::save_config()
{
    ui_.config().set("Connection type", connection_type == CT_EMULATOR ? "emulator" : "hardware");
    ui_.config().set("Serial port", serial_port_);
    ui_.config().save();
}

void Startup::draw()
{
    ImGui::SetNextWindowSize(ImVec2(550, 150));
    ImGui::Begin("Welcome to Fortuna debugger", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Select connection type");
    ImGui::RadioButton("Emulator", (int*) &connection_type, CT_EMULATOR); ImGui::SameLine();
    ImGui::RadioButton("Real hardware (serial)", (int*) &connection_type, CT_SERIAL);

    if (connection_type == CT_SERIAL) {
        ImGui::InputTextWithHint("Serial port", "/dev/devttyS0", serial_port_, IM_ARRAYSIZE(serial_port_)); ImGui::SameLine();
        if (ImGui::Button("Autodetect")) {
            std::string port = fdbg::DebuggerClient::auto_detect_port();
            strncpy(serial_port_, port.c_str(), sizeof(serial_port_));
        }
        ImGui::InputInt("Baud rate", &baud_rate_, 0);
    }

    // TODO - check for errors
    if (ImGui::Button("Connect")) {
        save_config();
        if (connection_type == CT_EMULATOR) {
            std::string port = ui_.emulator().init();
            ui_.emulator().run_as_thread();
            ui_.client().connect(port, fdbg::DebuggerClient::EMULATOR_BAUD);
        } else {
            ui_.client().connect(serial_port_, baud_rate_);
        }
        ui_.client().set_debugging_level(fdbg::DebuggingLevel::TRACE); // TODO
        ui_.client().ack_sync(MACHINE_ID);
        visible_ = false;
    }

    ImGui::End();
}
