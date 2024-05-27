#include "keypress.hh"

#include <GLFW/glfw3.h>

#include "keypress.hh"
#include "ui/keypress.hh"

void KeyPress::draw()
{
    if (ImGui::BeginPopupModal("Keypress", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press a key to send to the terminal.");
        ImGui::Separator();
        if (ImGui::Button("Cancel"))
            visible_ = false;
        ImGui::EndPopup();

        auto key = check_for_keypress();
        if (key) {
            model.terminal_model().next_tx = key;
            visible_ = false;
        }
    }

    ImGui::OpenPopup("Keypress");
}