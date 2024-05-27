#ifndef UI_HH_
#define UI_HH_

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "properties.hh"
#include "libfdbg-client.hh"
#include "windows/window.hh"
#include "model/model.hh"
#include "config/config.hh"

class UI {
public:
    explicit UI();
    ~UI();

    void run();

    void set_window_visible(std::string const& name, bool visible=true);
    void start_debugging_session();
    void update_theme();

private:
    template <typename W, typename... U> std::string add_window(bool visible=false, U... u) {
        auto w = std::make_unique<W>(visible, u...);
        std::string key = w->name();
        windows_[key] = std::move(w);
        return key;
    }

    struct GLFWwindow* glfw_window_ = nullptr;
    ImGuiContext*      context_;
    std::string        msg_box_key_;
    std::map<std::string, std::unique_ptr<Window>> windows_ {};
    size_t             blink_count_ = 0;
    size_t             theme_ = 0;
};

extern UI ui;

namespace Key {
    constexpr ImGuiKey PageUp = (ImGuiKey) 0x10a;
    constexpr ImGuiKey PageDown = (ImGuiKey) 0x10b;
    constexpr ImGuiKey F2 = (ImGuiKey) 0x123;
    constexpr ImGuiKey F6 = (ImGuiKey) 0x127;
    constexpr ImGuiKey F7 = (ImGuiKey) 0x128;
    constexpr ImGuiKey F8 = (ImGuiKey) 0x129;
    constexpr ImGuiKey F9 = (ImGuiKey) 0x12a;
    constexpr ImGuiKey F10 = (ImGuiKey) 0x12b;
    constexpr ImGuiKey F12 = (ImGuiKey) 0x12d;
}

#endif //UI_HH_
