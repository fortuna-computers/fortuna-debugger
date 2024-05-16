#include "code.hh"

#include "imgui.h"
#include "ui/ui.hh"

void Code::draw()
{
    if (ImGui::Begin("Debugger (code)", &visible_)) {
        draw_buttons();
        draw_code();
    }
    ImGui::End();
}

void Code::draw_buttons()
{
    /*
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button("Step (F7)") || ImGui::IsKeyPressed(Key::F7, true)) {
    }
    ImGui::SameLine();
    if (ImGui::Button("Next (F8)") || ImGui::IsKeyPressed(Key::F8, true)) {
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    if (ImGui::Button("Run (F9)") || ImGui::IsKeyPressed(Key::F9, false)) {
    }
    ImGui::SameLine();
     */

    // reset
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset (F2)") || ImGui::IsKeyPressed(Key::F2, false))
        model.reset();
    ImGui::PopStyleColor(3);

    // file dropdown
    ImGui::SetNextItemWidth(180.f);
    ImGui::Combo("##file", &selected_file_, model.files_cstr().data(), model.files_cstr().size());
}

void Code::draw_code()
{
    // table
    static int tbl_flags = ImGuiTableFlags_BordersOuterH
                           | ImGuiTableFlags_BordersOuterV
                           | ImGuiTableFlags_BordersInnerV
                           | ImGuiTableFlags_BordersOuter
                           | ImGuiTableFlags_RowBg
                           | ImGuiTableFlags_ScrollY
                           | ImGuiTableFlags_Resizable
                           | ImGuiTableFlags_Reorderable
                           | ImGuiTableFlags_Hideable;
    static ImU32 pc_row_color = ImGui::GetColorU32(ImVec4(0.3f, 0.7f, 0.3f, 0.65f));
    static ImU32 bkp_cell_color = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.2f, 0.65f));

    ImVec2 size = ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 42);
    if (ImGui::BeginTable("##code", 2, tbl_flags, size)) {

        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        size_t nline = 1;
        for (;;) {
            auto it = model.debug().source_lines.find(std::make_pair(selected_file_, nline));
            if (it == model.debug().source_lines.end())
                break;

            DebugInfo::SourceLine const& sl = it->second;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            std::string addr = model.fmt_addr(sl.address);

        }

        for (auto const& line: code_model_->lines()) {

            ImGui::TableNextRow();

            if (line.address.has_value()) {
                if (*line.address == emulator_.pc()) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, pc_row_color);
                    if (scroll_to_pc_) {
                        ImRect rect { ImGui::GetItemRectMin(), ImGui::GetItemRectMin() };
                        rect.Expand({ 0, 60 });
                        ImGui::ScrollToBringRectIntoView(ImGui::GetCurrentWindow(), rect);
                        scroll_to_pc_ = false;
                    }
                }

                bool is_breakpoint = emulator_.is_breakpoint(*line.address);

                if (is_breakpoint)
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, bkp_cell_color, 0);

                if (nline == show_this_line_on_next_frame_.value_or(-1)) {
                    ImGui::SetScrollHereY();
                    show_this_line_on_next_frame_.reset();
                }

                ImGui::TableSetColumnIndex(0);
                char buf[5];
                sprintf(buf, "%04X", *line.address);
                if (ImGui::Selectable(buf)) {
                    if (is_breakpoint)
                        code_model_->remove_breakpoint(nline);
                    else
                        code_model_->add_breakpoint(nline);
                }
            }

            ImGui::TableSetColumnIndex(1);
            char buf[256] = { 0 };
            int pos = 0;

            for (auto b: line.bytes) {
                if (pos > sizeof(buf) - 5)
                    break;
                pos += sprintf(&buf[pos], "%02X ", b);
            }

            if (!line.bytes.empty())
                ImGui::Text("%s", buf);

            ImGui::TableSetColumnIndex(2);

            if (line.comment_start) {
                ImGui::Text("%s", line.code.substr(0, *line.comment_start).c_str());
                ImGui::SameLine(0, 0);
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 1.0f, 1.0f), "%s", line.code.substr(*line.comment_start).c_str());
            } else {
                ImGui::Text("%s", line.code.c_str());
            }

            ++nline;
        }

        ImGui::EndTable();
    }
}