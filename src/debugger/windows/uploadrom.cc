#include "uploadrom.hh"

#include <string>
#include "imgui.h"

#include "ui/ui.hh"

using namespace std::string_literals;

void UploadROM::draw()
{
    if (model.upload_state()) {

        auto const& upload = model.upload_state().value();
        ImGui::OpenPopup("ROM upload");

        ImGui::SetNextWindowSize(ImVec2(400, 0));
        if (ImGui::BeginPopupModal("ROM upload")) {

            ImGui::Text("Uploading ROM...");
            ImGui::ProgressBar(upload.binary_idx / (float) upload.binary_count, ImVec2(-FLT_MIN, 0),
                   ("Binary: "s + std::to_string(upload.binary_idx) + " / " + std::to_string(upload.binary_count)).c_str());

            ImGui::ProgressBar(upload.current / (float) upload.total, ImVec2(-FLT_MIN, 0),
                               ("Bytes: "s + std::to_string(upload.current) + " / " + std::to_string(upload.total)).c_str());

            ImGui::Text("Address: 0x%s", model.fmt_addr(upload.address).c_str());

            ImGui::EndPopup();
        }

    }

}
