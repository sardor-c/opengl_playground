#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}


int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }

    std::fprintf(stderr, "GLFW: %s\n", glfwGetVersionString());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}