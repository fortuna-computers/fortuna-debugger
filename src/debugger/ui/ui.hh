#ifndef UI_HH_
#define UI_HH_

#include <map>
#include <memory>
#include <string>

#include "properties.hh"
#include "libfdbg-client.hh"
#include "windows/window.hh"
#include "model/model.hh"
#include "config/config.hh"

class UIInterface {
public:
    virtual DebuggerModel&     model() = 0;
    virtual IniPropertiesFile& ini_properties_file() = 0;
    virtual Config&            config() = 0;

    virtual void               init_debugging_session() = 0;
    virtual void               set_window_visible(std::string const& name, bool visible) = 0;

protected:
    UIInterface() = default;
};

class UI : public UIInterface {
public:
    explicit UI();
    ~UI();

    void run();

    DebuggerModel&      model() override { return model_; }
    IniPropertiesFile&  ini_properties_file() override { return ini_properties_file_; }
    Config&             config() override { return config_; }

    void set_window_visible(std::string const& name, bool visible) override;
    void init_debugging_session() override;

private:
    template <typename W>
    std::string add_window(bool visible=false) {
        auto w = std::make_unique<W>(*this, visible);
        std::string key = w->name();
        windows_[key] = std::move(w);
        return key;
    }

    DebuggerModel      model_;
    Config             config_;
    struct GLFWwindow* glfw_window_ = nullptr;
    ImGuiContext*      context_;
    IniPropertiesFile  ini_properties_file_;
    std::string        msg_box_key_;
    std::map<std::string, std::unique_ptr<Window>> windows_ {};
};

namespace Key {
    constexpr ImGuiKey PageUp = (ImGuiKey) 0x10a;
    constexpr ImGuiKey PageDown = (ImGuiKey) 0x10b;
    constexpr ImGuiKey F2 = (ImGuiKey) 0x123;
    constexpr ImGuiKey F7 = (ImGuiKey) 0x128;
    constexpr ImGuiKey F8 = (ImGuiKey) 0x129;
    constexpr ImGuiKey F9 = (ImGuiKey) 0x12a;
    constexpr ImGuiKey F10 = (ImGuiKey) 0x12b;
    constexpr ImGuiKey F12 = (ImGuiKey) 0x12d;
}

#endif //UI_HH_
