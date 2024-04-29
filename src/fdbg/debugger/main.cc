#include "fdbg.hh"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

int main()
{
    // initialize
    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW.");
    glfwSetErrorCallback([](int error, const char* description) {
        throw std::runtime_error("Error " + std::to_string(error) + ": " + description);
    });

    /*
    fdbg::DebuggerClient client("/dev/ttys001");
    client.set_debugging_level(fdbg::DebuggingLevel::TRACE);
    client.ack_sync(0x1234);
     */
}