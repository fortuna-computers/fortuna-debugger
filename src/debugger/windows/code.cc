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

        for (size_t nline = 1; ; ++nline) {
            auto it = model.debug().source_lines.find(std::make_pair(selected_file_, nline));
            if (it == model.debug().source_lines.end())
                break;

            DebugInfo::SourceLine const& sl = it->second;

            ImGui::TableNextRow();

            // address

            ImGui::TableSetColumnIndex(0);
            if (sl.address != DebugInfo::NO_ADDRESS) {
                if (sl.address == model.computer_status().pc()) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, pc_row_color);
                }

                std::string addr = model.fmt_addr(sl.address);
                if (ImGui::Selectable(addr.c_str())) {
                    // TODO - deal with breakpoint
                }
            }

            // line

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", sl.line.c_str());
            if (!sl.comment.empty()) {
                ImGui::SameLine(0, 0);
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 1.0f, 1.0f), "%s", sl.comment.c_str());
            }
        }

        ImGui::EndTable();
    }
}