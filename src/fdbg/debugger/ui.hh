#ifndef UI_HH_
#define UI_HH_

#include <map>
#include <memory>
#include <string>

#include "config.hh"
#include "fdbg.hh"
#include "windows/window.hh"

class UIInterface {
public:
    virtual fdbg::DebuggerClient& client() const = 0;
    virtual void                  set_window_visible(std::string const& name, bool visible) = 0;
    virtual Config&               config() = 0;

protected:
    UIInterface() = default;
};

class UI : public UIInterface {
public:
    explicit UI(fdbg::DebuggerClient& client);
    ~UI();

    void run();

    fdbg::DebuggerClient& client() const override { return client_; }
    Config&               config() override { return config_; }

    void set_window_visible(std::string const& name, bool visible) override;

private:
    template <typename W>
    void add_window(bool visible=false) {
        auto w = std::make_unique<W>(*this, visible);
        windows_[w->name()] = std::move(w);
    }

    fdbg::DebuggerClient&                          client_;
    struct GLFWwindow*                             glfw_window_ = nullptr;
    Config                                         config_;
    std::map<std::string, std::unique_ptr<Window>> windows_ {};
};

#endif //UI_HH_
