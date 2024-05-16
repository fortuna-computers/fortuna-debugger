#include "configuration.hh"

#include "imgui.h"

#include "ui/ui.hh"

void Configuration::draw()
{
    ImGui::SetNextWindowSize(ImVec2(400, 200));
    ImGui::Begin("Configurations", &visible_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
    ImGui::BeginTabBar("configurations_tab");

    ImGui::BeginTabItem("Initialization");

    static bool verify = model.config().get_bool("verify_upload_rom");
    if (ImGui::Checkbox("Verify memory when uploading ROM", &verify)) {
        model.config().set("verify_upload_rom", verify);
    }

    ImGui::EndTabItem();

    ImGui::EndTabBar();
    ImGui::End();
}