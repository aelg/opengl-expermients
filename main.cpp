#include <GL/glew.h>

#include "Context.h"
#include "Model.h"
#include "load_shaders.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;


Context initWindow(int width, int height){
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(width, height, "Tutorial 01", nullptr, nullptr);
    if(window == nullptr){
        cerr << "Failed to open window" << endl;
        glfwTerminate();
        return Context(nullptr, width, height);
    }
    glfwMakeContextCurrent(window);
    glewExperimental= static_cast<GLboolean>(true);
    if(glewInit() != GLEW_OK){
        cerr << "Failed to initialize GLEW" << endl;
    }
    return Context(window, width, height);
}

void initKeyboard(Context const &context){
    glfwSetInputMode(context.getWindow(), GLFW_STICKY_KEYS, GL_TRUE);
}

void initVao(Context &context){
    glGenVertexArrays(1, &context.vertexArrayId);
    glBindVertexArray(context.vertexArrayId);
}

void initBuffers(Model &model, State &state, Shader &shader){
    /*static const GLfloat g_vertexbuffer_data[]={
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
    };
    glGenBuffers(1, &context.vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, context.vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexbuffer_data), g_vertexbuffer_data, GL_STATIC_DRAW);
     */
    makeTriangle(model, state, shader, "triangle1", {0, 0, 0});
    makeTriangle(model, state, shader, "triangle2", {0, 1, 0});


    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
}

void loadShaders(Shader &shader){
    shader = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/fragment_shader.glsl");
}

void initCamera(Context &context, State &state){
    state.projection = glm::perspective(
            glm::radians(90.0f),
            static_cast<float>(context.width)/ static_cast<float>(context.height),
            0.1f,
            100.0f
    );
}

void updateState(State &state){
    Triangle &t1 = state.get<Triangle>("triangle1");
    t1.rot += 0.01f;
    Triangle &t2 = state.get<Triangle>("triangle2");
    t2.rot -= 0.01f;
}

void setCamera(State &state, Shader &shader){
    glm::mat4 view = glm::lookAt(
            glm::vec3(0, 0, 3), // Camera pos.
            glm::vec3(0, 0, 0), // Looking at.
            glm::vec3(0, 1, 0)  // Up-direction.
    );

    mat4 vp = state.projection * view;
    glUniformMatrix4fv(shader.vp_matrix_id, 1, GL_FALSE, &vp[0][0]);
}

void view(Context const& context, Model &model, State &state, Shader &shader){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.program_id);
    glEnableVertexAttribArray(0);
    setCamera(state, shader);
    model.draw(context, state);
    glDisableVertexAttribArray(0);
}

void mainLoop(Context &context, Model &model, State &state, Shader &shader){
    for(;;){
        updateState(state);
        view(context, model, state, shader);
        glfwSwapBuffers(context.getWindow());
        glfwPollEvents();
        if(glfwGetKey(context.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) return;
        if(glfwWindowShouldClose(context.getWindow()) != 0) return;
    }
}

int main() {

    if(!glfwInit()){
        cerr << "Failed to initialize glfw" << endl;
    }
    auto context = initWindow(640, 640);
    State state;
    Model model;
    Shader shader;
    initVao(context);
    initKeyboard(context);
    loadShaders(shader);
    initBuffers(model, state, shader);
    initCamera(context, state);
    mainLoop(context, model, state, shader);

    return 0;
}