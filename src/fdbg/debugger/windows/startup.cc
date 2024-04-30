#include "startup.hh"

#include <imgui.h>

void Startup::draw()
{
    ImGui::SetNextWindowSize(ImVec2(600, 82));
    ImGui::Begin("Welcome to Fortuna debugger", nullptr, ImGuiWindowFlags_NoResize);


    ImGui::End();
}
