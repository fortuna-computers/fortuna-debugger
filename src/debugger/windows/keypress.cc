#include "keypress.hh"

#include <GLFW/glfw3.h>

void KeyPress::draw()
{
    if (ImGui::BeginPopupModal("Keypress", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press a key to send to the terminal.");
        ImGui::Separator();
        if (ImGui::Button("Cancel"))
            visible_ = false;
        ImGui::EndPopup();

        if (do_keypress())
            visible_ = false;
    }

    ImGui::OpenPopup("Keypress");
}

bool KeyPress::do_keypress()
{
    ImGuiIO const& io = ImGui::GetIO();

    bool press = false;
    std::string key;

    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
        if (ImGui::IsKeyPressed((ImGuiKey) i)) {
            for (char k: translate_keypress(i, io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper))
               key += k;
            press = true;
        }
    }

    if (press)
        model.terminal_model().next_tx = key;
    else if (!press || key.empty())
        model.terminal_model().next_tx = {};

    return press;
}

std::vector<uint8_t> KeyPress::translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super)
{
    if (ctrl || alt || super)   // not supported yet
        return {};
    if (key >= 'A' && key <= 'Z') {
        return { shift ? (uint8_t) key : (uint8_t) (key + 32) };
    } else if (key < 127) {
        return { (uint8_t) key };
    } else switch(key) {
        case GLFW_KEY_ENTER: return { '\r', '\n' };
        case GLFW_KEY_BACKSPACE: return { '\b' };
        default: return {};
    }

    return {};
}