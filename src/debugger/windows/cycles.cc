#include "cycles.hh"

#include <ranges>

#include "imgui.h"

#include "model/model.hh"

void Cycles::draw()
{
    if (ImGui::Begin("Cycles", &visible_)) {
        disable_on_run([&]() {
            if (ImGui::Button("Cycle"))
                model.cycle();
            ImGui::SameLine();
        });
        if (ImGui::Button("Clear"))
            model.clear_cycles();

        if (ImGui::BeginTable("##cycles", 1 + model.machine().cycle_bytes.size() + model.machine().cycle_bits.size(), ImGuiTableFlags_Borders)) {

            // headers

            ImGui::TableSetupColumn("#");
            for (auto const& bytes: model.machine().cycle_bytes)
                ImGui::TableSetupColumn(bytes.name.c_str());
            for (auto const& bit: model.machine().cycle_bits)
                ImGui::TableSetupColumn(bit.c_str());
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();

            // data

            size_t counter = model.cycles().size() - 1;
            for (auto it = model.cycles().begin(); it != model.cycles().end(); ++it) {

                auto const& cycle = *it;

                int k = 0;

                // counter
                ImGui::TableSetColumnIndex(k++);
                ImGui::Text("%zu", counter--);

                // bytes
                for (int j = 0; j < cycle.bytes_size(); ++j) {
                    ImGui::TableSetColumnIndex(k++);
                    uint field_sz = model.machine().cycle_bytes.at(j).size;
                    if (cycle.bytes(j).has()) {
                        ImGui::Text("%0*llX", field_sz / 4, cycle.bytes(j).value());
                    } else {
                        char unknown[] = "--------";
                        unknown[field_sz / 4] = '\0';
                        ImGui::Text("%s", unknown);
                    }
                }

                // bits

                for (int j = 0; j < cycle.bits_size(); ++j) {
                    ImGui::TableSetColumnIndex(k++);
                    if (cycle.bits(j)) {
                        ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "1");
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "0");
                    }
                }

                ImGui::TableNextRow();
            }

        }
        ImGui::EndTable();

        save_window_closed(visible_);
    }

    ImGui::End();
}
