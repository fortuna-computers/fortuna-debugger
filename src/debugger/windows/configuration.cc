#include "configuration.hh"

#include "imgui.h"

#include "ui/ui.hh"

void Configuration::draw()
{
    ImGui::SetNextWindowSize(ImVec2(400, 200));
    ImGui::Begin("Configurations", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
    ImGui::BeginTabBar("configurations_tab");

    ImGui::BeginTabItem("Initialization");

    static bool verify = model.config().verify_upload_rom;
    if (ImGui::Checkbox("Verify memory after uploading ROM", &verify)) {
        model.config().verify_upload_rom = verify;
        model.config().save();
    }

    ImGui::EndTabItem();

    ImGui::EndTabBar();
    ImGui::End();
}