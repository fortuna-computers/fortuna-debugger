#include "window.hh"

#include "model/model.hh"

bool Window::blink = false;

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

void Window::save_window_closed(bool visible)
{
    if (!visible && is_debugging_window())
        model.config().remove(name() + "_window_visible");
}