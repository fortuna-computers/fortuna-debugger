#include "startup.hh"

#include <imgui.h>

#include "ui/ui.hh"


Startup::Startup(bool visible)
    : Window(visible)
{
    if (model.config().get_str("connection_type") == "hardware")
        connection_type = CT_SERIAL;

    strncpy(machine_path_, model.config().get_str("machine_path").c_str(), IM_ARRAYSIZE(machine_path_));
    strncpy(emulator_path_, model.config().get_str("emulator_path").c_str(), IM_ARRAYSIZE(emulator_path_));
    strncpy(serial_port_, model.config().get_str("serial_port").c_str(), IM_ARRAYSIZE(serial_port_));
    strncpy(source_file_, model.config().get_str("source_file").c_str(), IM_ARRAYSIZE(source_file_));

    if (machine_path_[0] != '\0') {
        try {
            model.load_machine(machine_path_);
        } catch (std::exception& e) {
            machine_path_[0] = '\0';
        }
    }

    file_browser_.SetTitle("Choose machine file");
    source_browser_.SetTitle("Choose source file");
}

void Startup::save_config()
{
    model.config().set("machine_path", machine_path_);
    model.config().set("connection_type", connection_type == CT_EMULATOR ? "emulator" : "hardware");
    model.config().set("serial_port", serial_port_);
    model.config().set("emulator_path", emulator_path_);
    model.config().set("source_file", source_file_);
    model.config().save();
}

void Startup::draw()
{
    ImGui::SetNextWindowSize(ImVec2(800, 340));
    ImGui::Begin("Welcome to Fortuna debugger", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::SeparatorText("Machine");

    ImGui::Text("Select machine definition file");
    if (ImGui::Button("Load machine"))
        file_browser_.Open();
    ImGui::SameLine();
    ImGui::Text("%s", machine_path_);

    if (machine_path_[0] != '\0') {

        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Machine: %s", model.machine().name.c_str());

        ImGui::SeparatorText("Connection");

        ImGui::Text("Select connection type");
        ImGui::RadioButton("Emulator", (int*) &connection_type, CT_EMULATOR); ImGui::SameLine();
        ImGui::RadioButton("Real hardware (serial)", (int*) &connection_type, CT_SERIAL);

        if (connection_type == CT_EMULATOR) {
            if (ImGui::Button("Emulator path"))
                emulator_browser_.Open();
            ImGui::SameLine();
            ImGui::Text("%s", emulator_path_);
        } else if (connection_type == CT_SERIAL) {
            ImGui::InputTextWithHint("Serial port", "/dev/ttyS0", serial_port_, IM_ARRAYSIZE(serial_port_)); ImGui::SameLine();
            if (ImGui::Button("Autodetect")) {
                std::string port =  FdbgClient::autodetect_usb_serial_port(model.machine().vendor_id, model.machine().product_id);
                strncpy(serial_port_, port.c_str(), sizeof(serial_port_));
            }
            ImGui::InputInt("Baud rate", &baud_rate_, 0);
        }

        ImGui::SeparatorText("Source");
        ImGui::Text("Select source file");
        if (ImGui::Button("Select file"))
            source_browser_.Open();
        ImGui::SameLine();
        ImGui::Text("%s", source_file_);

        ImGui::SeparatorText("Connect");

        // TODO - disable if fields are not filled out
        if (ImGui::Button("Connect & Upload")) {
            save_config();
            model.compile(source_file_);
            if (connection_type == CT_EMULATOR)
                model.connect_to_emulator(emulator_path_);
            else
                model.connect_to_serial_port(serial_port_, baud_rate_);

            visible_ = false;
            model.init_debugging_session();
        }
    }

    ImGui::End();

    file_browser_.Display();
    if (file_browser_.HasSelected()) {
        strncpy(machine_path_, file_browser_.GetSelected().c_str(), sizeof(machine_path_));
        file_browser_.ClearSelected();
        model.load_machine(machine_path_);
        baud_rate_ = model.machine().uc_baudrate;
    }

    source_browser_.Display();
    if (source_browser_.HasSelected()) {
        strncpy(source_file_, source_browser_.GetSelected().c_str(), sizeof(source_file_));
        source_browser_.ClearSelected();
    }

    emulator_browser_.Display();
    if (emulator_browser_.HasSelected()) {
        strncpy(emulator_path_, emulator_browser_.GetSelected().c_str(), sizeof(emulator_path_));
        emulator_browser_.ClearSelected();
    }
}
