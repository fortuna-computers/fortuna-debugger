#include "mainmenu.hh"

#include "imgui.h"

#include "ui/ui.hh"

using namespace std::string_literals;

static ImVec2 sz = ImVec2(-FLT_MIN, 0.0f);

void MainMenu::draw()
{
    ImGui::SetNextWindowSize(ImVec2(180, 0));
    ImGui::Begin("Main menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    if (model.debugging_session_started()) {
        ImGui::SeparatorText("Debugger");

        button("Debugger (code)", "code");
        button("Registers", "registers");
        uint8_t i = 0;
        for (auto const& memory: model.machine().memories)
            button("Memory ("s + memory.name + ")", "memory" + std::to_string(i++));
        button("Terminal", "terminal");
        button("Interrupt", "interrupt");
        button("Cycles", "cycles");
        button("Debug Text", "debugtext");
    }

    ImGui::SeparatorText("Config");

    button("Configurations", "config");
#ifdef DEBUG
    button("Imgui Demo", "demo");
#endif

    if (ImGui::Button("Quit", sz))
        exit(0);  // TODO - ask for confirmation

    ImGui::End();
}

void MainMenu::button(std::string const &text, std::string const &window_name)
{
    if (ImGui::Button(text.c_str(), sz))
        ui.set_window_visible(window_name, true);
}
