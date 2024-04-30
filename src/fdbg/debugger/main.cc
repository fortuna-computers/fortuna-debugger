#include "fdbg.hh"

#include "ui.hh"
#include "windows/window.hh"
#include "windows/demo.hh"

static GLFWwindow* window = nullptr;
static std::vector<std::unique_ptr<Window>> windows;

int main()
{
    fdbg::DebuggerClient client;
    UI ui(client);
    ui.run();
}