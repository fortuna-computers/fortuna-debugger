#ifndef WINDOW_HH_
#define WINDOW_HH_

#include <string>

#include "imgui.h"
#include "model/model.hh"

class Window {
public:
    explicit Window(bool visible=false) : visible_(visible) {}
    virtual ~Window() = default;

    [[nodiscard]] bool visible() const { return visible_; }
    void set_visible(bool visible);

    void reopen_debugging_session();

    // implement this
    virtual void draw() = 0;
    virtual std::string name() const = 0;

    virtual void update() {}
    virtual bool is_debugging_window() const { return false; }

protected:
    template<typename F> void disable_on_run(F f) {
        if (model.running()) ImGui::BeginDisabled();
        f();
        if (model.running()) ImGui::EndDisabled();
    }

    bool visible_;
};

#endif //WINDOW_HH_
