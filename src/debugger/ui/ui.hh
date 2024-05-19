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

class UI {
public:
    explicit UI();
    ~UI();

    void run();

    void set_window_visible(std::string const& name, bool visible);
    void start_debugging_session();

private:
    template <typename W> std::string add_window(bool visible=false) {
        auto w = std::make_unique<W>(visible);
        std::string key = w->name();
        windows_[key] = std::move(w);
        return key;
    }

    struct GLFWwindow* glfw_window_ = nullptr;
    ImGuiContext*      context_;
    std::string        msg_box_key_;
    std::map<std::string, std::unique_ptr<Window>> windows_ {};
    size_t             blink_count_ = 0;
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
