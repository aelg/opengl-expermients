//
// Created by aelg on 5/14/17.
//

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "Model.h"

using namespace std;
using namespace glm;


void Model::draw(Context const &context, State const &state) {
    for(auto &object : objects){
        object.second(context, state);
    }
}

void Model::add(std::string name, draw_function draw) {
    objects[name] = draw;
}

void makeTriangle(Model &model, State &state, Shader& shader, std::string name, Triangle const &initial_state){
    static const GLfloat g_vertexbuffer_data[]={
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
    };
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexbuffer_data), g_vertexbuffer_data, GL_STATIC_DRAW);

    state.add(name, initial_state);
    model.add(name, [vertexbuffer, shader, name](Context const &context, State const &state){
        Triangle &triangle = state.get<Triangle>(name);

        mat4 t = translate(vec3{triangle.x, triangle.y, 0.0f});
        mat4 r = rotate(triangle.rot, vec3{0.0f, 0.0f, 1.0f});
        mat4 m = t * r;

        glUniformMatrix4fv(shader.m_matrix_id, 1, GL_FALSE, &m[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    });
}