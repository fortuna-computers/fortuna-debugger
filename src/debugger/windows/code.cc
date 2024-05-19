#include "code.hh"

#include "imgui.h"
#include "imgui_internal.h"

#include "ui/ui.hh"

void Code::draw()
{
    if (ImGui::Begin("Debugger (code)", &visible_)) {
        draw_buttons();
        draw_code();
        draw_footer();
    }
    ImGui::End();
}

void Code::draw_buttons()
{
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button("Step (F7)") || ImGui::IsKeyPressed(Key::F7, true)) {
        model.step(true);
        scroll_to_addr_in_next_frame_ = model.computer_status().pc();
    }
    ImGui::SameLine();
    if (ImGui::Button("Next (F8)") || ImGui::IsKeyPressed(Key::F8, true)) {
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    if (ImGui::Button("Run (F9)") || ImGui::IsKeyPressed(Key::F9, false)) {
    }
    ImGui::SameLine();

    // reset
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset (F2)") || ImGui::IsKeyPressed(Key::F2, false)) {
        model.reset();
        scroll_to_addr_in_next_frame_ = model.computer_status().pc();
    }
    ImGui::PopStyleColor(3);

    // more
    ImGui::SameLine(0.0f, 30.f);
    ImGui::Checkbox("More...", &show_more_);

    // file dropdown
    ImGui::SetNextItemWidth(180.f);
    ImGui::Combo("##file", &selected_file_, model.files_cstr().data(), model.files_cstr().size());

    // symbol dropdown
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if (ImGui::Combo("##symbol", &selected_symbol_, model.symbols_cstr().data(), model.symbols_cstr().size())) {
        try {
            scroll_to_addr_in_next_frame_ = model.debug().symbols.at(model.symbols_cstr().at(selected_symbol_));
        } catch (std::out_of_range&) {}
        selected_symbol_ = 0;
    }
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

    ImVec2 size = ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 19 - (show_more_ ? 23 :0 ));
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

                bool is_bkp = model.breakpoints().contains(sl.address);

                // colors
                if (sl.address == model.computer_status().pc())
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, pc_row_color);
                if (is_bkp)
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, bkp_cell_color, 0);

                // scroll to row
                if (scroll_to_addr_in_next_frame_ && scroll_to_addr_in_next_frame_ == sl.address) {
                    ImRect rect { ImGui::GetItemRectMin(), ImGui::GetItemRectMin() };
                    rect.Expand({ 0, 40 });
                    ImGui::ScrollToBringRectIntoView(ImGui::GetCurrentWindow(), rect);
                    scroll_to_addr_in_next_frame_ = {};
                }

                // address
                std::string addr = model.fmt_addr(sl.address);
                if (ImGui::Selectable(addr.c_str())) {
                    if (is_bkp)
                        model.remove_breakpoint(sl.address);
                    else
                        model.add_breakpoint(sl.address);
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

void Code::draw_footer()
{
    if (show_more_) {
        if (ImGui::Button("Simple step")) {
            model.step(false);
            scroll_to_addr_in_next_frame_ = model.computer_status().pc();
        }
        ImGui::SameLine();
        if (ImGui::Button("Run forever"))
            ;
        ImGui::SameLine();
        if (ImGui::Button("Clear bkps"))
            model.clear_breakpoints();
    }

    ImGui::Text("Click on the address to set a breakpoint.");
}