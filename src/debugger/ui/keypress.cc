#include "keypress.hh"

#include "imgui.h"
#include <GLFW/glfw3.h>

#include <vector>

static std::vector<uint8_t> translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super)
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
}

std::optional<std::string> check_for_keypress()
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

    if (!press || key.empty())
        return {};
    else
        return key;
}