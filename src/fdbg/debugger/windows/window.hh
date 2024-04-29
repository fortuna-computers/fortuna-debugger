#ifndef WINDOW_HH_
#define WINDOW_HH_

#include "fdbg.hh"

class Window {
public:
    explicit Window(fdbg::DebuggerClient& client) : client_(client) {}
    virtual ~Window() = default;

    [[nodiscard]] bool visible() const { return visible_; }
    virtual void set_visible(bool visible) { visible_ = visible; }

    // implement this
    virtual void draw() = 0;
    virtual void update() {}
    [[nodiscard]] virtual std::string name() const = 0;

protected:
    fdbg::DebuggerClient client_;
    bool visible_ = false;

    static const int PageUp = 0x10a;
    static const int PageDown = 0x10b;
    static const int F2 = 0x123;
    static const int F7 = 0x128;
    static const int F8 = 0x129;
    static const int F9 = 0x12a;
    static const int F10 = 0x12b;
    static const int F12 = 0x12d;
};

#endif //WINDOW_HH_
