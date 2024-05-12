#include "ui.hh"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "exceptions/exceptions.hh"
#include "windows/window.hh"
#include "windows/demo.hh"
#include "windows/mainmenu.hh"
#include "windows/messagebox.hh"
#include "windows/startup.hh"
#include "windows/memory.hh"
#include "windows/configuration.hh"
#include "windows/uploadrom.hh"

UI::UI()
    : config_(ini_properties_file_)
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::string glsl_version = "#version 150";

    // create window
    glfw_window_ = glfwCreateWindow(1200, 800, "Fortuna debugger", nullptr, nullptr);

    // setup OpenGL
    glfwMakeContextCurrent(glfw_window_);
    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD.");
    glClearColor(0, 0, 0, 1.0f);

    // configure GLFW
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(glfw_window_, [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });

    // configure ImGui
    IMGUI_CHECKVERSION();
    context_ = ImGui::CreateContext();
    ImGuiIO* io_ = &ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(glfw_window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    ImGui::StyleColorsDark();
    io_->KeyRepeatRate = 0.1f;

    // .ini config
    ini_properties_file_.initialize(context_);
    config_.load();

    // add windows
    add_window<MainMenu>(true);
    add_window<Startup>(true);
    add_window<Configuration>();
    add_window<Demo>();
    add_window<Memory>();
    add_window<UploadROM>(true);
    msg_box_key_ = add_window<MessageBox>();
}

UI::~UI()
{
    // terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (glfw_window_)
        glfwDestroyWindow(glfw_window_);
    glfwTerminate();
}

void UI::run()
{
    while (!glfwWindowShouldClose(glfw_window_)) {

        model_.update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto const& [_, w]: windows_) {
            if (w->visible()) {
                try {
                    w->draw();
                } catch (std::exception& e) {
                    while (context_->CurrentWindowStack.Size > 1)
                        ImGui::End();
                    fprintf(stderr, "%s\n", e.what());
                    ((MessageBox *) windows_.at(msg_box_key_).get())->set_message(MessageBox::Type::Error, e.what());
                }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfw_window_);

        glfwPollEvents();
    }
}

void UI::set_window_visible(std::string const& name, bool visible)
{
    windows_.at(name)->set_visible(visible);
}

void UI::init_debugging_session()
{
    set_window_visible("memory", true);
    model_.initialize_memory();
}
