//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Context {
    GLFWwindow *window;
public:
    GLuint vertexArrayId;
    int width;
    int height;
    explicit Context(int width, int height);

    void initWindow(std::string const &window_name);
    void initVao();

    GLFWwindow *getWindow() const;
};


#endif //OPENGL_CONTEXT_H
