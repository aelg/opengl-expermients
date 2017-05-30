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

static void calcNormals(GLfloat const *vertices, GLfloat *normals, int size){
    for(int i = 0; i < size; i += 9){
        GLfloat const *c1 = &vertices[i];
        GLfloat const *c2 = &vertices[i+3];
        GLfloat const *c3 = &vertices[i+6];
        vec3 edge1 = vec3{c2[0], c2[1], c2[2]} - vec3{c1[0], c1[1], c1[2]};
        vec3 edge2 = vec3{c3[0], c3[1], c3[2]} - vec3{c1[0], c1[1], c1[2]};
        vec3 normal = normalize(cross(edge1, edge2));
        for(int j = 0; j < 3; ++j){
            GLfloat *n = &normals[i + j*3];
            n[0] = normal.x;
            n[1] = normal.y;
            n[2] = normal.z;
        }
    }
}


void Model::draw(Context const &context, State const &state, GLuint program) {
    for(auto &object : objects){
        object.second(context, state, program);
    }
}

void Model::add(std::string name, draw_function draw) {
    objects[name] = draw;
}

class Destroyer{
private:
    std::function<void()> f;
    Destroyer(){}
public:
    Destroyer(std::function<void()> destroyer) : f(destroyer){}
    Destroyer(Destroyer &destroyer){
        f = destroyer.f;
        destroyer.f = nullptr;
    }
    Destroyer(Destroyer &&destroyer){
        f = destroyer.f;
        destroyer.f = nullptr;
    }
    Destroyer &operator=(Destroyer&) = delete;
    Destroyer &operator=(Destroyer&&) = delete;
    ~Destroyer(){
        if(f) f();
        cout << "Destroyer called" << endl;
    }
};

void makeTriangle(Model &model, State &state, unique_ptr<Shader> &shaderp, std::string const &name, Triangle const &initial_state){
    static const GLfloat g_vertexbuffer_data[]={
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
    };
    static const GLfloat g_colorbuffer_data[] = {
            1.0f,  0.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  0.0f,  0.1f,
    };
    static GLfloat g_normalbuffer_data[3*3*1];
    calcNormals(g_vertexbuffer_data, g_normalbuffer_data, 3*3*1);
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexbuffer_data), g_vertexbuffer_data, GL_STATIC_DRAW);
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_colorbuffer_data), g_colorbuffer_data, GL_STATIC_DRAW);
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_normalbuffer_data), g_normalbuffer_data, GL_STATIC_DRAW);

    state.add(name, initial_state);
    model.add(name, [vertexbuffer, colorbuffer, normalbuffer, &shaderp, name](Context const &context, State const &state, GLuint program){
        auto &shader = *shaderp;
        if(program != shader.program_id) return;
        Triangle &triangle = state.get<Triangle>(name);

        mat4 t = translate(vec3{triangle.x, triangle.y, 0.0f});
        mat4 r = rotate(triangle.rot, vec3{0.0f, 0.0f, 1.0f});
        mat4 m = t * r;
        mat4 mvp = state.projection * state.view * m;

        glUseProgram(shader.program_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glUniformMatrix4fv(shader["MVP"], 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(shader["M"], 1, GL_FALSE, &m[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    });
}

void makeCube(Model &model, State &state, unique_ptr<Shader> &shaderp, std::string const &name, Cube const &initial_state){
    static const GLfloat g_vertexbuffer_data[]={
            -1.0f,-1.0f,-1.0f, // triangle 1 : begin
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, // triangle 1 : end
            1.0f, 1.0f,-1.0f, // triangle 2 : begin
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f, // triangle 2 : end
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f
    };
    static const GLfloat g_colorbuffer_data[] = {
            0.583f,  0.771f,  0.014f,
            0.609f,  0.115f,  0.436f,
            0.327f,  0.483f,  0.844f,
            0.822f,  0.569f,  0.201f,
            0.435f,  0.602f,  0.223f,
            0.310f,  0.747f,  0.185f,
            0.597f,  0.770f,  0.761f,
            0.559f,  0.436f,  0.730f,
            0.359f,  0.583f,  0.152f,
            0.483f,  0.596f,  0.789f,
            0.559f,  0.861f,  0.639f,
            0.195f,  0.548f,  0.859f,
            0.014f,  0.184f,  0.576f,
            0.771f,  0.328f,  0.970f,
            0.406f,  0.615f,  0.116f,
            0.676f,  0.977f,  0.133f,
            0.971f,  0.572f,  0.833f,
            0.140f,  0.616f,  0.489f,
            0.997f,  0.513f,  0.064f,
            0.945f,  0.719f,  0.592f,
            0.543f,  0.021f,  0.978f,
            0.279f,  0.317f,  0.505f,
            0.167f,  0.620f,  0.077f,
            0.347f,  0.857f,  0.137f,
            0.055f,  0.953f,  0.042f,
            0.714f,  0.505f,  0.345f,
            0.783f,  0.290f,  0.734f,
            0.722f,  0.645f,  0.174f,
            0.302f,  0.455f,  0.848f,
            0.225f,  0.587f,  0.040f,
            0.517f,  0.713f,  0.338f,
            0.053f,  0.959f,  0.120f,
            0.393f,  0.621f,  0.362f,
            0.673f,  0.211f,  0.457f,
            0.820f,  0.883f,  0.371f,
            0.982f,  0.099f,  0.879f
    };
    static GLfloat g_normalbuffer_data[3*3*12];
    calcNormals(g_vertexbuffer_data, g_normalbuffer_data, 3*3*12);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexbuffer_data), g_vertexbuffer_data, GL_STATIC_DRAW);
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_colorbuffer_data), g_colorbuffer_data, GL_STATIC_DRAW);
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_normalbuffer_data), g_normalbuffer_data, GL_STATIC_DRAW);

    state.add(name, initial_state);
    model.add(name, [vertexbuffer, colorbuffer, normalbuffer, &shaderp, name](Context const &context, State const &state, GLuint program){
        auto &shader = *shaderp;
        if(program != shader.program_id) return;
        Triangle &triangle = state.get<Triangle>(name);

        mat4 t = translate(vec3{triangle.x, triangle.y, 0.0f});
        mat4 r = rotate(triangle.rot, vec3{1.0f, 1.0f, 1.0f});
        mat4 m = t * r;
        mat4 mvp = state.projection * state.view * m;

        glUseProgram(shader.program_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glUniformMatrix4fv(shader["MVP"], 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(shader["M"], 1, GL_FALSE, &m[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    });
}



void makeSquare(Model &model, State &state, unique_ptr<Shader> &shaderp, std::string const &name, GLfloat size, Square const &initial_state){
    static const GLfloat g_vertexbuffer_data[]={
            size, size, 0.0f,
            -size, size, 0.0f,
            size, -size, 0.0f,
            -size, -size, 0.0f,
    };
    static const GLfloat g_colorbuffer_data[] = {
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
    };
    static GLfloat g_normalbuffer_data[] = {
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
    };
    static GLfloat g_uvbuffer_data[] = {
            1, 1,
            0, 1,
            1, 0,
            0, 0,
    };
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexbuffer_data), g_vertexbuffer_data, GL_STATIC_DRAW);
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_colorbuffer_data), g_colorbuffer_data, GL_STATIC_DRAW);
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_normalbuffer_data), g_normalbuffer_data, GL_STATIC_DRAW);
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uvbuffer_data), g_uvbuffer_data, GL_STATIC_DRAW);

    state.add(name, initial_state);
    model.add(name, [vertexbuffer, colorbuffer, normalbuffer, uvbuffer, &shaderp, name](Context const &context, State const &state, GLuint program){
        auto &shader = *shaderp;
        if(program != shader.program_id) return;
        Square &square = state.get<Square>(name);

        mat4 t = translate(vec3{square.x, square.y, 0.0f});
        mat4 s = scale(vec3{square.width, square.height, 1.0f});
        mat4 m = t * s;
        mat4 mvp = state.projection * state.view * m;

        GLfloat time = static_cast<GLfloat>(glfwGetTime());

        glUseProgram(shader.program_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glUniformMatrix4fv(shader["MVP"], 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(shader["M"], 1, GL_FALSE, &m[0][0]);
        glUniform1fv(shader["time_vertex"], 1, &time);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
    });
}