#include <GL/glew.h>

#include "Context.h"
#include "Model.h"
#include "InputHandler.h"
#include "load_shaders.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <unistd.h>

using namespace glm;
using namespace std;

using Shaders = map<string, unique_ptr<Shader>>;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Context initWindow(int width, int height) {
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(width, height, "Mandelbrot", nullptr, nullptr);
    if (window == nullptr) {
        cerr << "Failed to open window" << endl;
        glfwTerminate();
        return Context(nullptr, width, height);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = static_cast<GLboolean>(true);
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
    }
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return Context(window, width, height);
}

void initKeyboard(Context const &context) {
    glfwSetInputMode(context.getWindow(), GLFW_STICKY_KEYS, GL_TRUE);
}

void initVao(Context &context) {
    glGenVertexArrays(1, &context.vertexArrayId);
    glBindVertexArray(context.vertexArrayId);
}

void initBuffers(Model &model, State &state, Shaders &shaders) {
    makeSquare(model, state, shaders["julia"], "julia", 0.5, {0.25, 0.5, 0.5, 1});
    makeSquare(model, state, shaders["mandelbrot"], "mandelbrot", 0.5, {0.75, 0.5, 0.5, 1});


    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void loadShaders(Shaders &shaders) {
    shaders["standard"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/fragment_shader.glsl");
    shaders["julia"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/julia_fragment_shader.glsl");
    shaders["mandelbrot"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/mandelbrot_fragment_shader.glsl");
}

struct Camera {
    vec3 pos;
    vec3 direction;
    vec3 up;
    float vert = 0;
    float horiz = 0;
    double mouseSpeed = 0.05;
    double speed = 4;
    double lastTime;
    bool captureMouse = true;
};

void initCamera(Context &context, State &state, InputHandler &inputHandler) {
    double curTime = glfwGetTime();
    /*state.projection = glm::perspective(
            glm::radians(40.0f),
            static_cast<float>(context.width)/ static_cast<float>(context.height),
            0.1f,
            100.0f
    );*/
    state.projection = glm::ortho(
            0.0f,
            1.0f,
            0.0f,
            1.0f,
            -0.1f,
            1.0f
    );
    //glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetCursorPos(context.getWindow(), context.width / 2, context.height / 2);
    state.view = mat4();
}

void updateState(Context const &context, State &state) {
    glfwPollEvents();
    if (glfwWindowShouldClose(context.getWindow()) != 0) state.doQuit = true;
}

void setCamera(State &state, Shader &shader) {
    glUniformMatrix4fv(shader.v_matrix_id, 1, GL_FALSE, &state.view[0][0]);
    glUniformMatrix4fv(shader.p_matrix_id, 1, GL_FALSE, &state.projection[0][0]);
}

void setOffset(Context const &context, Shader &shader) {
    double x, y;
    float a[2];
    glfwGetCursorPos(context.getWindow(), &x, &y);
    int width, height;
    glfwGetWindowSize(context.getWindow(), &width, &height);
    x = x/width;
    y = 1-y/height;
    x = x - 0.5;
    x = x*2;
    a[0] = static_cast<float>(x);
    a[1] = static_cast<float>(y);
    if(x >= 0) glUniform2fv(shader.offset_vertex_id, 1, a);
}

void view(Context const &context, Model &model, State &state, Shaders &shaders) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &shader : shaders) {
        setCamera(state, *shader.second);
        setOffset(context, *shader.second);
        model.draw(context, state, shader.second->program_id);
    }
}

void updateShader(Shaders &shaders, State &state) {
    if (state.updateShader) {
        loadShaders(shaders);
        state.updateShader = false;
    }
}

void initInput(InputHandler &inputHandler, State &state, Context &context) {
    inputHandler.addHandler({GLFW_KEY_R, GLFW_PRESS, GLFW_MOD_CONTROL}, [&state]() { state.updateShader = true; });
    inputHandler.addHandler({GLFW_KEY_ESCAPE, GLFW_PRESS, 0}, [&state]() { state.doQuit = true; });
    inputHandler.addHandler({GLFW_KEY_Q, GLFW_PRESS, 0}, [&state, &context]() {
        auto &camera = state.get<Camera>("camera");
        camera.captureMouse = !camera.captureMouse;
        if (camera.captureMouse) {
            glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(context.getWindow(), context.width / 2, context.height / 2);
        } else {
            glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    });
    glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void mainLoop(Context &context, Model &model, State &state, Shaders &shaders) {
    for (; !state.doQuit;) {
        updateState(context, state);
        view(context, model, state, shaders);
        glfwSwapBuffers(context.getWindow());
        updateShader(shaders, state);
    }
}

int main() {

    if (!glfwInit()) {
        cerr << "Failed to initialize glfw" << endl;
    }
    auto context = initWindow(640, 640);
    State state;
    Model model;
    Shaders shaders;
    auto inputHandler = InputHandler::makeInputHandler(context.getWindow());
    initVao(context);
    initKeyboard(context);
    loadShaders(shaders);
    initBuffers(model, state, shaders);
    initCamera(context, state, *inputHandler);
    initInput(*inputHandler, state, context);
    mainLoop(context, model, state, shaders);

    return 0;
}