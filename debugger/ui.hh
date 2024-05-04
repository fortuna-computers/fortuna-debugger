#ifndef UI_HH_
#define UI_HH_

#include <map>
#include <memory>
#include <string>

#include "config.hh"
#include "libfdbg-client.hh"
#include "emulator/emulator.hh"
#include "windows/window.hh"
#include "model.hh"

class UIInterface {
public:
    virtual FdbgClient&    client() = 0;
    virtual DebuggerModel& model() = 0;
    virtual void           set_window_visible(std::string const& name, bool visible) = 0;
    virtual Config&        config() = 0;
    virtual Emulator&      emulator() = 0;

protected:
    UIInterface() = default;
};

class UI : public UIInterface {
public:
    explicit UI();
    ~UI();

    void run();

    FdbgClient& client() override { return client_; }
    DebuggerModel& model() override { return model_; }
    Emulator&   emulator() override { return emulator_; }
    Config&     config() override { return config_; }

    void set_window_visible(std::string const& name, bool visible) override;

private:
    template <typename W>
    std::string add_window(bool visible=false) {
        auto w = std::make_unique<W>(*this, visible);
        std::string key = w->name();
        windows_[key] = std::move(w);
        return key;
    }

    FdbgClient         client_;
    DebuggerModel      model_;
    Emulator           emulator_;
    struct GLFWwindow* glfw_window_ = nullptr;
    Config             config_;
    std::string        msg_box_key_;
    std::map<std::string, std::unique_ptr<Window>> windows_ {};
};

#endif //UI_HH_
