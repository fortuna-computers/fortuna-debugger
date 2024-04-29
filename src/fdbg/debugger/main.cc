#include "fdbg.hh"

#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "windows/window.hh"
#include "windows/demo.hh"

static GLFWwindow* window = nullptr;
static std::vector<std::unique_ptr<Window>> windows;

static void initialize_ui()
{
    // initialize
    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW.");
    glfwSetErrorCallback([](int error, const char* description) {
        throw std::runtime_error("Error " + std::to_string(error) + ": " + description);
    });

    // window hints
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::string glsl_version = "#version 150";

    // create window
    window = glfwCreateWindow(1200, 800, "Fortuna debugger", nullptr, nullptr);

    // setup OpenGL
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD.");
    glClearColor(0, 0, 0, 1.0f);

    // configure GLFW
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });

    // configure ImGui
    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO* io_ = &ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    ImGui::StyleColorsDark();
    io_->KeyRepeatRate = 0.1f;
}

static void destroy_ui()
{
    // terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
}

static void loop_cycle()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto const& w: windows)
        w->draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    glfwPollEvents();
}

int main()
{
    fdbg::DebuggerClient client;

    windows.push_back(std::make_unique<Demo>(client));

    initialize_ui();

    // main loop
    while (!glfwWindowShouldClose(window)) {
        loop_cycle();
    }

    destroy_ui();

    /*
    fdbg::DebuggerClient client("/dev/ttys001");
    client.set_debugging_level(fdbg::DebuggingLevel::TRACE);
    client.ack_sync(0x1234);
     */
}