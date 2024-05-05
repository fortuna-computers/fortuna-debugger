#include "startup.hh"

#include <imgui.h>

#include "../ui.hh"
#include "debugger/load.hh"


Startup::Startup(UIInterface& ui, bool visible)
    : Window(ui, visible)
{
    if (ui.config().get("Connection type") == "hardware")
        connection_type = CT_SERIAL;

    strncpy(machine_path_, ui.config().get("Machine path").c_str(), IM_ARRAYSIZE(machine_path_));
    strncpy(serial_port_, ui.config().get("Serial port").c_str(), IM_ARRAYSIZE(serial_port_));

    if (machine_path_[0] != '\0') {
        try {
            load_machine(machine_path_);
        } catch (std::exception& e) {
            machine_path_[0] = '\0';
        }
    }

    file_browser_.SetTitle("Choose machine file");
    file_browser_.SetTypeFilters({ ".so", ".dylib" });
}

void Startup::save_config()
{
    ui_.config().set("Machine path", machine_path_);
    ui_.config().set("Connection type", connection_type == CT_EMULATOR ? "emulator" : "hardware");
    ui_.config().set("Serial port", serial_port_);
    ui_.config().save();
}

void Startup::draw()
{
    ImGui::SetNextWindowSize(ImVec2(800, 250));
    ImGui::Begin("Welcome to Fortuna debugger", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::SeparatorText("Machine");

    ImGui::Text("Select machine");
    if (ImGui::Button("Load machine"))
        file_browser_.Open();
    ImGui::SameLine();
    ImGui::Text("%s", machine_path_);

    if (machine_path_[0] != '\0') {

        ImGui::Text("Machine: %s", machine_name());

        ImGui::SeparatorText("Connection");

        ImGui::Text("Select connection type");
        ImGui::RadioButton("Emulator", (int*) &connection_type, CT_EMULATOR); ImGui::SameLine();
        ImGui::RadioButton("Real hardware (serial)", (int*) &connection_type, CT_SERIAL);

        if (connection_type == CT_SERIAL) {
            ImGui::InputTextWithHint("Serial port", "/dev/devttyS0", serial_port_, IM_ARRAYSIZE(serial_port_)); ImGui::SameLine();
            if (ImGui::Button("Autodetect")) {
                std::string port = FdbgClient::autodetect_usb_serial_port(microcontroller_vendor_id(), microcontroller_product_id());
                strncpy(serial_port_, port.c_str(), sizeof(serial_port_));
            }
            ImGui::InputInt("Baud rate", &baud_rate_, 0);
        }

        if (ImGui::Button("Connect")) {
            save_config();
            if (connection_type == CT_EMULATOR)
                ui_.model().connect_to_emulator();
            else
                ui_.model().connect_to_serial_port(serial_port_, baud_rate_);

            visible_ = false;
            ui_.init_debugging_session();
        }
    }

    ImGui::End();

    file_browser_.Display();
    if (file_browser_.HasSelected()) {
        strncpy(machine_path_, file_browser_.GetSelected().c_str(), sizeof(machine_path_));
        file_browser_.ClearSelected();
        load_machine(machine_path_);
    }

}
