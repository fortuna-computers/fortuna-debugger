#include "keypress.hh"

void KeyPress::draw()
{
    if (ImGui::BeginPopupModal("Keypress", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press a key to send to the terminal.");
        ImGui::Separator();
        if (ImGui::Button("Cancel"))
            visible_ = false;
        ImGui::EndPopup();

        if (do_keypress())
            visible_ = false;
    }

    if (visible_)
        ImGui::OpenPopup("Keypress");
}

bool KeyPress::do_keypress()
{
    ImGuiIO const& io = ImGui::GetIO();
    key_ = "";

    bool press = false;
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
        if (ImGui::IsKeyPressed((ImGuiKey) i)) {
            for (char k: translate_keypress(i, io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper))
                (*key_) += k;
            press = true;
        }
    }
    return press;
}
