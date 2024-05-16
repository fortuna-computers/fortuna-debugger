#include "window.hh"

#include "model/model.hh"

void Window::set_visible(bool visible)
{
    visible_ = visible;
    if (is_debugging_window())
        model.config().set(name() + "_window_visible", visible);
}

void Window::reopen_debugging_session()
{
    if (is_debugging_window() && model.config().get_bool(name() + "_window_visible"))
        set_visible(true);
}
