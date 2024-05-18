#include "registers.hh"

#include "imgui.h"

#include "model/model.hh"

void Registers::draw()
{
    ImGui::Begin("Registers", &visible_, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SeparatorText("Registers");

    if (ImGui::BeginTable("##regs", model.machine().registers.size(), ImGuiTableFlags_Borders)) {
        for (auto const& reg: model.machine().registers)
            ImGui::TableSetupColumn(reg.name.c_str(), 0);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        for (size_t i = 0; i < model.machine().registers.size(); ++i) {
            auto const& reg = model.machine().registers.at(i);
            ImGui::TableSetColumnIndex((int) i);
            if ((size_t) model.computer_status().registers_size() <= i) {
                char unknown[] = "????????";
                unknown[reg.size / 4] = '\0';
                ImGui::Text("%s", unknown);
            } else {
                ImGui::Text("%0*llX", reg.size / 4, model.computer_status().registers(i));
            }
        }
    }
    ImGui::EndTable();

    ImGui::SeparatorText("Flags");

    if (model.computer_status().flags_size() == 0) {
        ImGui::Text("Not up to date");
    } else {
        for (size_t i = 0; i < model.machine().flags.size(); ++i) {
            bool v = false;
            if (i < (size_t) model.computer_status().flags_size())
                v = model.computer_status().flags(i);
            ImGui::Checkbox(model.machine().flags.at(i).c_str(), &v);
            ImGui::SameLine();
        }
    }

    ImGui::End();
}