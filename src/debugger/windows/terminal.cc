#include "terminal.hh"

#include "model/model.hh"
#include "ui/ui.hh"

Terminal::Terminal(bool visible)
    : Window(visible)
{
    mode_ = model.terminal_model().mode();
    new_line_ = model.terminal_model().new_line();
}

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
                if (m.cursor().x == x && m.cursor().y == y) {
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

        if (model.running())
            ImGui::BeginDisabled(true);

        // next char
        {
            char buf[100];
            int n = snprintf(buf, sizeof buf, "Next TX: ");
            if (!model.terminal_model().next_tx) {
                if (ImGui::Button("Keypress..."))
                    ui.set_window_visible("keypress");
                ImGui::SameLine();
                snprintf(&buf[n], sizeof buf - n, "(none)");
            } else {
                if (ImGui::Button("Clear"))
                    model.terminal_model().next_tx = {};
                ImGui::SameLine();
                for (char c: *model.terminal_model().next_tx)
                    n += snprintf(&buf[n], sizeof buf - n, "%c", (c >= 32 && c < 127) ? c : '?');
                n += snprintf(&buf[n], sizeof buf - n, " (");
                for (char c: *model.terminal_model().next_tx)
                    n += snprintf(&buf[n], sizeof buf - n, "0x%02X ", c);
                snprintf(&buf[n-1], sizeof buf - n, ")");
            }
            ImGui::Text("%s", buf);
        }

        if (model.running())
            ImGui::EndDisabled();

        ImGui::SeparatorText("Configuration");

        // options
        if (ImGui::Combo("Mode", (int *) &mode_, "ANSI\0Raw\0\0", ImGuiComboFlags_WidthFitPreview))
            model.terminal_model().set_mode(mode_);
        if (mode_ == TerminalModel::M_RAW)
            if (ImGui::Combo("New line", (int *) &new_line_, "CR (13)\0LF (10)\0CR+LF (13+10)\0\0", ImGuiComboFlags_WidthFitPreview))
                model.terminal_model().set_new_line(new_line_);

        // buttons
        if (mode_ == TerminalModel::M_RAW) {
            if (ImGui::Button("Clear screen"))
                model.terminal_model().clear();
        } else {
            if (ImGui::Button("Reset terminal"))
                model.terminal_model().reset();
        }

        ImGui::End();

        save_window_closed(visible_);
    }
}