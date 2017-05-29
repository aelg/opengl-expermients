//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Context {
    GLFWwindow *window;
public:
    GLuint vertexArrayId;
    GLuint vertexbuffer;
    GLuint programId;
    //GLint shader_vp_matrix_id;
    //GLint shader_m_matrix_id;
    glm::mat4 projection;
    int width;
    int height;
    explicit Context(GLFWwindow* window, int width, int height);

    GLFWwindow *getWindow() const;
};


#endif //OPENGL_CONTEXT_H
