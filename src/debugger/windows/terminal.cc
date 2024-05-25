#include "terminal.hh"

#include "model/model.hh"

void Terminal::draw()
{
    auto const& m = model.terminal_model();

    ImVec2 topleft, bottomright;

    if (ImGui::Begin("Terminal", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {

        // draw text
        auto [lines, columns] = m.size();
        for (size_t y = 0; y < lines; ++y) {
            for (size_t x = 0; x < columns; ++x) {

                // draw character
                auto const& ch = m.chr(y, x);
                if (x != 0)
                    ImGui::SameLine(0.0f, 0.0f);
                ImGui::Text("%c", ch.c);  // TODO - color

                // draw cursor
                if (m.cursor() == std::make_pair(y, x)) {
                    auto br = ImGui::GetItemRectMax(); br.x += 2; br.y++;
                    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), br, IM_COL32(0, 196, 0, 128));
                }

                // get positions for drawing the border
                if (x == 0 && y == 0)
                    topleft = ImGui::GetItemRectMin();
                else if (x == columns - 1 && y == lines - 1)
                    bottomright = ImGui::GetItemRectMax();
            }
        }

        // draw borders
        topleft.x -= 2; topleft.y -= 1; bottomright.x += 4; bottomright.y += 2;
        ImGui::GetWindowDrawList()->AddRect(topleft, bottomright, IM_COL32(128, 128, 128, 255));

        // options
        if (ImGui::Combo("Mode", (int *) &mode_, "ANSI\0Raw\0\0", ImGuiComboFlags_WidthFitPreview))
            model.terminal_model().set_mode(mode_);
        if (mode_ == TerminalModel::M_RAW)
            if (ImGui::Combo("New line", (int *) &new_line_, "CR (13)\0LF (10)\0CR+LF (13+10)\0\0", ImGuiComboFlags_WidthFitPreview))
                model.terminal_model().set_new_line(new_line_);

        // buttons
        if (ImGui::Button("Clear screen"))
            model.terminal_model().clear();

        ImGui::End();
    }
}