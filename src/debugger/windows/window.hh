#ifndef WINDOW_HH_
#define WINDOW_HH_

#include <string>

class Window {
public:
    explicit Window(bool visible=false) : visible_(visible) {}
    virtual ~Window() = default;

    [[nodiscard]] bool visible() const { return visible_; }
    virtual void set_visible(bool visible) { visible_ = visible; }

    // implement this
    virtual void draw() = 0;
    virtual void update() {}
    [[nodiscard]] virtual std::string name() const = 0;

protected:
    bool visible_;
};

#endif //WINDOW_HH_
