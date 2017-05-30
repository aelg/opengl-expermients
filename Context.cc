//
// Created by aelg on 5/14/17.
//

#include "Context.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

using namespace std;


static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Context::Context(int width, int height) :
        window(nullptr),
        vertexArrayId(0),
        width(width),
        height(height) {

}

void Context::initWindow(string const &window_name) {
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        throw runtime_error("Failed to open window.");
    }
    glfwMakeContextCurrent(window);
    glewExperimental = static_cast<GLboolean>(true);
    if (glewInit() != GLEW_OK) {
        throw runtime_error("Failed to initialize GLEW");
    }
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}


void Context::initVao() {
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);
}

GLFWwindow *Context::getWindow() const {
    return window;
}