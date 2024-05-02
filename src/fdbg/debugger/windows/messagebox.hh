#ifndef MESSAGEBOX_HH_
#define MESSAGEBOX_HH_

#include "window.hh"

class MessageBox : public Window {
public:
    enum Type { Info, Error, FatalError };

    using Window::Window;

    void set_message(Type type, std::string const& message, std::string const& title = "");
    void draw() override;
    std::string name() const override { return "message_box"; }

private:
    std::string message_;
    std::string title_;
    Type        type_ = Type::Info;
};

#endif //MESSAGEBOX_HH_
