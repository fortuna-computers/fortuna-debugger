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
        save_window_closed(visible_);
    }
    ImGui::End();
}

void Code::draw_buttons()
{
    bool next = false;

    disable_on_run([&]() {
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button("Step (F7)") || ImGui::IsKeyPressed(Key::F7, true))
            model.step(true);
        ImGui::SameLine();
        if (ImGui::Button("Next (F8)") || ImGui::IsKeyPressed(Key::F8, true))
            next = true;  // do it out of the lambda
        ImGui::PopButtonRepeat();
    });

    if (next)
        model.next();

    ImGui::SameLine();
    if (model.running()) {
        if (ImGui::Button("Pause (F9)") || ImGui::IsKeyPressed(Key::F9, false))
            model.pause();
    } else {
        if (ImGui::Button("Run (F9)") || ImGui::IsKeyPressed(Key::F9, false))
            model.run(false);
    }

    // reset
    disable_on_run([&]() {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Reset (F2)") || ImGui::IsKeyPressed(Key::F2, false))
            model.reset();
        ImGui::PopStyleColor(3);
    });

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
            uint64_t address = model.debug().symbols.at(model.symbols_cstr().at(selected_symbol_));
            model.scroll_to_line_ = model.debug().addresses.at(address);
        } catch (std::out_of_range&) {}
        selected_symbol_ = 0;
    }
}

void Code::draw_code()
{
    if (model.scroll_to_line_)
        selected_file_ = model.scroll_to_line_->first;

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

                // address
                std::string addr = model.fmt_addr(sl.address);
                if (ImGui::Selectable(addr.c_str()) && !model.running()) {
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

            // scroll to row

            if (model.scroll_to_line_ && model.scroll_to_line_->second == nline) {
                ImRect rect { ImGui::GetItemRectMin(), ImGui::GetItemRectMin() };
                rect.Expand({ 0, 40 });
                ImGui::ScrollToBringRectIntoView(ImGui::GetCurrentWindow(), rect);
            }

        }

        ImGui::EndTable();
    }

    model.scroll_to_line_.reset();
}

void Code::draw_footer()
{
    disable_on_run([&]() {
        if (show_more_) {
            if (ImGui::Button("Simple step"))
                model.step(false);
            ImGui::SameLine();
            if (ImGui::Button("Run forever"))
                model.run(true);
            ImGui::SameLine();
            if (ImGui::Button("Clear bkps"))
                model.clear_breakpoints();
        }
    });

    if (!model.running())
        ImGui::Text("Click on the address to set a breakpoint.");
    else if (Window::blink)
        ImGui::Text("Running...");
}