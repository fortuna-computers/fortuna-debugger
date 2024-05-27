#include "keypress.hh"

#include "imgui.h"
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <vector>

static std::unordered_map<ImGuiKey, std::string> key_translation = {
        { ImGuiKey_Escape, "\e" },
        { ImGuiKey_Enter, "\r\n" },
        { ImGuiKey_Backspace, "\b" },
};

std::optional<std::string> check_for_keypress()
{
    ImGuiIO const& io = ImGui::GetIO();

    std::string ret;

    // TODO - modifiers

    for (auto const& [key, str]: key_translation)
        if (ImGui::IsKeyPressed(key))
            ret += str;

    for (int i = 0; i < io.InputQueueCharacters.Size; ++i) {
        ImWchar c = io.InputQueueCharacters[i];
        char buf[8] = {0};
        wctomb(buf, c);
        ret += buf;
    }

    return ret.empty() ? std::optional<std::string>{} : ret;
}