//
// Created by aelg on 5/14/17.
//

#include "Context.h"

Context::Context(GLFWwindow *window, int width, int height) :
        window(window),
        vertexArrayId(0),
        width(width),
        height(height)
{}

GLFWwindow *Context::getWindow() const {
    return window;
}