//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_LOAD_SHADERS_H
#define OPENGL_LOAD_SHADERS_H

#include <string>
#include <GL/glew.h>
#include <memory>

struct Shader{
    GLuint program_id = static_cast<GLuint>(-1);
    GLint mvp_matrix_id;
    GLint m_matrix_id;
    GLint v_matrix_id;
    GLint p_matrix_id;
    GLint lightposition_id;
    GLint time_vertex_id;
    ~Shader();
};

std::unique_ptr<Shader> load_shaders(std::string const &vertex_file_path, std::string const &fragment_file_path);

#endif //OPENGL_LOAD_SHADERS_H
