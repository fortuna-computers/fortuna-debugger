#include "debugtext.hh"

void DebugText::draw()
{
    if (ImGui::Begin("Debug text", &visible_)) {
        if (ImGui::Button("Clear"))
            model.clear_debug_text();

        ImGui::Separator();

        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
            ImGui::TextUnformatted(model.debug_text().c_str());
        ImGui::EndChild();

        save_window_closed(visible_);
    }

    ImGui::End();
}