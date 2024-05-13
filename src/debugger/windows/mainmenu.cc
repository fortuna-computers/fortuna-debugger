#include "mainmenu.hh"

#include "imgui.h"

#include "ui/ui.hh"

static ImVec2 sz = ImVec2(-FLT_MIN, 0.0f);

void MainMenu::draw()
{
    ImGui::SetNextWindowSize(ImVec2(180, 0));
    ImGui::Begin("Main menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    if (ui_.model().connected()) {
        ImGui::SeparatorText("Debugger");

        button("Memory", "memory");
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
        ui_.set_window_visible(window_name, true);
}