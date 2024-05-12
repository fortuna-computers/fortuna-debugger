#include "properties.hh"

#include <stdexcept>

#include "imgui_internal.h"

void IniPropertiesFile::initialize(struct ImGuiContext *context)
{
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "UserData";
    ini_handler.TypeHash = ImHashStr("UserData");
    ini_handler.UserData = &properties_;
    ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* h, const char* name) -> void* {
        if (strcmp(name, "Config") == 0)
            return h->UserData;
        return nullptr;
    };
    ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(entry);
        std::string sline = line;
        if (sline.find('=') != std::string::npos) {
            std::string key = sline.substr(0, sline.find('='));
            std::string value = sline.substr(sline.find('=') + 1);
            props[key] = value;
        }
    };
    ini_handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* h, ImGuiTextBuffer* buf) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(h->UserData);
        buf->appendf("[UserData][Config]\n");
        for (auto& [k,v] : props)
            buf->appendf((k + "=%s\n").c_str(), v.c_str());
    };
    context->SettingsHandlers.push_back(ini_handler);

    ImGui::LoadIniSettingsFromDisk("imgui.ini");
}

std::string IniPropertiesFile::get(std::string const &key) const
{
    try {
        return properties_.at(key);
    } catch (std::out_of_range&) {
        return "";
    }
}

void IniPropertiesFile::set(std::string const &key, std::string const &value)
{
    properties_[key] = value;
}

bool IniPropertiesFile::get_bool(std::string const& key) const
{
    return get(key) == "true";
}

void IniPropertiesFile::set(std::string const& key, bool value)
{
    set(key, (std::string) { value ? "true" : "false" });
}

void IniPropertiesFile::save()
{
    ImGui::SaveIniSettingsToDisk("imgui.ini");
}
