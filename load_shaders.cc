//
// Created by aelg on 5/14/17.
//

#include "load_shaders.h"

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

string read_file(string const &filename){
    string code;
    ifstream stream(filename, ios::in);
    if(stream.is_open()){
        string line;
        while(getline(stream, line)) code += "\n" + line;
        stream.close();
    }
    else{
        cerr << "Can't open " << filename << endl;
    }
    return code;
}

void compile_shader(string code, GLuint shader_id, string name){
    GLint result = GL_FALSE;
    int info_log_length;

    cout << "Compiling shader: " << name << endl;
    char const *s = code.c_str();
    glShaderSource(shader_id, 1, &s, nullptr);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0){
        vector<char> error_message(static_cast<size_t>(info_log_length+1));
        glGetShaderInfoLog(shader_id, info_log_length, nullptr, &error_message[0]);
        cerr << &error_message[0] << endl;
    }
}

GLuint link_program(vector<GLuint> const &shaders){
    GLint result = GL_FALSE;
    int info_log_length = 0;

    cout << "Linking program" << endl;
    GLuint program_id = glCreateProgram();
    for(auto shader : shaders){
        glAttachShader(program_id, shader);
    }
    glLinkProgram(program_id);

    glGetShaderiv(program_id, GL_LINK_STATUS, &result);
    glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0){
        vector<char> error_message(static_cast<size_t>(info_log_length+1));
        glGetShaderInfoLog(program_id, info_log_length, nullptr, &error_message[0]);
        cerr << &error_message[0] << endl;
    }

    for(auto shader : shaders){
        glDetachShader(program_id, shader);
    }
    return program_id;
}

unique_ptr<Shader> load_shaders(string const &vertex_file_path,
                                string const &fragment_file_path,
                                vector<std::string> const &uniforms){
    GLuint program_id;
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    string vertex_shader_code = read_file(vertex_file_path);
    string fragment_shader_code = read_file(fragment_file_path);

    compile_shader(vertex_shader_code, vertex_shader_id, vertex_file_path);
    compile_shader(fragment_shader_code, fragment_shader_id, fragment_file_path);

    program_id = link_program({vertex_shader_id, fragment_shader_id});

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    Shader *shader = new Shader();
    for(auto &uniform : uniforms){
        shader->uniform[uniform] = glGetUniformLocation(program_id, uniform.c_str());
    }
    shader->program_id = program_id;

    return unique_ptr<Shader>(shader);
}

Shader::~Shader(){
    if(program_id != static_cast<GLuint>(-1)) {
        glDeleteProgram(program_id);
        cout << "Delete program " << program_id << endl;
    }
}

GLint Shader::operator[](std::string const &name) const {
    auto it = uniform.find(name);
    if(it == uniform.end()) throw runtime_error("Using unregistered uniform");
    return it->second;
}
