//
// Created by 15813 on 2026/7/14.
//

#include "HWindow.h"
#include <iostream>

namespace UI {
    void Window::HWindowsInit() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    }

    GLFWwindow* Window::HWindowsCreate(const char *WindowTitle, int x, int y) {
        GLFWwindow *window = glfwCreateWindow(x, y,WindowTitle, nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }
        return window;
    }

    void Window::HWindowsFrameInit() {
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
    }

    void Window::HWindowsFrameEnd(GLFWwindow* window, ImGuiIO& io) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
