#include "messagebox.hh"

#include "imgui.h"

void MessageBox::set_message(MessageBox::Type type, std::string const& message, std::string const& title)
{
    type_ = type;
    message_ = message;
    if (!title.empty()) {
        title_ = title;
    } else switch (type) {
            case Info: title_ = "Information"; break;
            case Error: title_ = "Error"; break;
            case FatalError: title_ = "Fatal Error"; break;
        }
    set_visible(true);
}

void MessageBox::draw()
{
    if (type_ == Error || type_ == FatalError)
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::BeginPopupModal(title_.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", message_.c_str());
        ImGui::Separator();
        if (ImGui::Button("Ok")) {
            set_visible(false);
            if (type_ == FatalError)
                exit(1);
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();

    ImGui::OpenPopup(title_.c_str());
}