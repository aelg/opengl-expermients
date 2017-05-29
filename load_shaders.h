//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_LOAD_SHADERS_H
#define OPENGL_LOAD_SHADERS_H

#include <string>
#include <GL/glew.h>

struct Shader{
    GLuint program_id;
    GLint vp_matrix_id;
    GLint m_matrix_id;
};

Shader load_shaders(std::string const &vertex_file_path, std::string const &fragment_file_path);

#endif //OPENGL_LOAD_SHADERS_H
