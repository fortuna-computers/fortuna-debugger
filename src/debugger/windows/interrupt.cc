#include "interrupt.hh"

void Interrupt::draw()
{
    if (ImGui::Begin("Interrupt", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::InputInt("Interrupt number", &number_, 1, 10, ImGuiInputTextFlags_CharsHexadecimal);

        if (ImGui::Button("Fire interrupt!"))
            model.interrupt(number_);
        save_window_closed(visible_);
    }

    ImGui::End();
}