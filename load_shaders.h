//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_LOAD_SHADERS_H
#define OPENGL_LOAD_SHADERS_H

#include <string>
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <map>

struct Shader{
    GLuint program_id = static_cast<GLuint>(-1);
    std::map<std::string, GLint> uniform;
    GLint operator[](std::string const &name) const;
    ~Shader();
};

std::unique_ptr<Shader> load_shaders(std::string const &vertex_file_path,
                                     std::string const &fragment_file_path,
                                     std::vector<std::string> const &uniforms);

#endif //OPENGL_LOAD_SHADERS_H
