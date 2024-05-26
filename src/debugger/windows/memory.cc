#include "memory.hh"

#include "imgui.h"

#include "ui/ui.hh"

void Memory::draw()
{
    auto const& memory = model.memories.at(nr_);

    float h = 370;
    ImGui::SetNextWindowSize(ImVec2(560, h));
    if (ImGui::Begin(title_.c_str(), &visible_, ImGuiWindowFlags_NoResize)) {

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Page: (PgUp)");
        ImGui::SameLine();

        disable_on_run([&]() {
            if (ImGui::Button("<") || (!ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(Key::PageUp)))
                go_to_page_number(((int64_t) memory.current_page) - 1);
            ImGui::SameLine();

            char buf[10];
            snprintf(buf, 10, "%02zX", memory.current_page);
            ImGui::PushItemWidth(48.0f);
            ImGui::InputText("##page", buf, sizeof buf, ImGuiInputTextFlags_CallbackEdit,
                             [](ImGuiInputTextCallbackData* data) {
                                 auto* m = reinterpret_cast<Memory*>(data->UserData);
                                 unsigned long new_page = strtoul(data->Buf, nullptr, 16);
                                 if (new_page != ULONG_MAX)
                                     m->go_to_page_number(new_page);
                                 return 0;
                             }, this);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button(">") || (!ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(Key::PageDown)))
                go_to_page_number(memory.current_page + 1);
            ImGui::SameLine();
            ImGui::Text("(PgDown)");
        });

        draw_memory_table();
        if (nr_ == 0)
            draw_stack();
    }
    ImGui::End();
}

void Memory::draw_memory_table() const
{
    auto const& memory = model.memories.at(nr_);

    static int tbl_flags = ImGuiTableFlags_BordersOuter
                           | ImGuiTableFlags_NoBordersInBody
                           | ImGuiTableFlags_RowBg
                           | ImGuiTableFlags_ScrollY;

    static ImU32 pc_bg_color = ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.2f, 0.65f));

    ImVec2 size = ImVec2(-FLT_MIN, 293);
    if (ImGui::BeginTable("##ram", 18, tbl_flags, size)) {

        // headers
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < 0x10; ++i) {
            char buf[3];
            snprintf(buf, 3, "_%X", i);
            ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed);
        }
        ImGui::TableSetupColumn("ASCII", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (int line = 0; line < Model::PAGE_SZ / 0x10; ++line) {
            ImGui::TableNextRow();

            // address
            uint16_t addr = (memory.current_page << 8) + (line * 0x10);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%04X : ", addr);

            // data
            std::string ascii;
            for (uint8_t i = 0; i < 0x10; ++i) {
                ImGui::TableSetColumnIndex(i + 1);
                std::optional<uint8_t> byte = memory.data[line * 16 + i];
                if (byte.has_value()) {
                    bool needs_pop = false;
                    if (addr + i == model.computer_status().pc())
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, pc_bg_color);
                    if (*byte == 0) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(128, 128, 128)));
                        needs_pop = true;
                    }
                    ImGui::Text("%02X", *byte);
                    if (needs_pop)
                        ImGui::PopStyleColor();
                    ascii += (*byte >= 32 && *byte < 127) ? (char) *byte : '.';
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(128, 128, 128)));
                    ImGui::Text("??");
                    ImGui::PopStyleColor();
                    ascii += '.';
                }
            }

            // ascii
            ImGui::TableSetColumnIndex(17);
            ImGui::Text("%s", ascii.c_str());
        }

        ImGui::EndTable();
    }
}

void Memory::draw_stack() const
{
    char stack[512] = "Stack: ";
    size_t n = strlen(stack);
    for (unsigned char data : model.computer_status().stack())
        n += snprintf(&stack[n], sizeof stack - n, "%02X ", data);
    ImGui::Text("%s", stack);
}

void Memory::go_to_page_number(int64_t page)
{
    model.change_memory_page(nr_, page);
}
