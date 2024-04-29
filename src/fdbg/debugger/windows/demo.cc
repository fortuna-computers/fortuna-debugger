#include "demo.hh"

#include "imgui.h"

void Demo::draw()
{
    ImGui::ShowDemoWindow(&visible_);
}
