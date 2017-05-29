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
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    return Context(window, width, height);
}

void initKeyboard(Context const &context){
    glfwSetInputMode(context.getWindow(), GLFW_STICKY_KEYS, GL_TRUE);
}

void initVao(Context &context){
    glGenVertexArrays(1, &context.vertexArrayId);
    glBindVertexArray(context.vertexArrayId);
}

void initBuffers(Model &model, State &state, Shaders &shaders){
    makeTriangle(model, state, shaders["standard"], "triangle", {0, -2, 0});
    makeCube(model, state, shaders["standard"], "cube", {0, 2, 0});
    makeSquare(model, state, shaders["julia"], "julia", 2, {4, 0, 1, 1});
    makeSquare(model, state, shaders["mandelbrot"], "mandelbrot", 2, {8, 0, 1, 1});


    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void loadShaders(Shaders &shaders){
    shaders["standard"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/fragment_shader.glsl");
    shaders["julia"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/julia_fragment_shader.glsl");
    shaders["mandelbrot"] = load_shaders(SHADER_DIR"/vertex_shader.glsl", SHADER_DIR"/mandelbrot_fragment_shader.glsl");
}

struct Camera{
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

void initCamera(Context &context, State &state, InputHandler &inputHandler){
    double curTime = glfwGetTime();
    state.projection = glm::perspective(
            glm::radians(40.0f),
            static_cast<float>(context.width)/ static_cast<float>(context.height),
            0.1f,
            100.0f
    );
    /*state.projection = glm::ortho(
            -10.0f,
            10.0f,
            -10.0f,
            10.0f,
            0.1f,
            100.0f
    );//*/
    Camera camera{};
    camera.pos = {0.0f, 0.0f, 10.0f};
    camera.horiz = 3.14f;
    camera.vert = 0;
    camera.lastTime = curTime;
    state.add("camera", camera);
    glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(context.getWindow(), context.width/2, context.height/2);
}

void updateCamera(Context const &context, State &state){
    Camera &camera = state.get<Camera>("camera");
    double xpos, ypos;
    double curTime = glfwGetTime();
    double deltaTime = curTime - camera.lastTime;
    if(camera.captureMouse) {
        glfwGetCursorPos(context.getWindow(), &xpos, &ypos);
        glfwSetCursorPos(context.getWindow(), context.width / 2, context.height / 2);
        camera.horiz += (context.width / 2 - xpos) * camera.mouseSpeed * deltaTime;
        camera.vert += (context.height / 2 - ypos) * camera.mouseSpeed * deltaTime;
        camera.direction = {
                glm::cos(camera.vert) * sin(camera.horiz),
                glm::sin(camera.vert),
                glm::cos(camera.vert) * cos(camera.horiz)
        };
    }
    glm::vec3 right{glm::cross(camera.direction, vec3{0,1,0})};
    camera.up = {glm::cross(right, camera.direction)};
    glm::vec3 movement{0.0f};
    if(glfwGetKey(context.getWindow(), GLFW_KEY_W) == GLFW_PRESS) movement += camera.direction;
    if(glfwGetKey(context.getWindow(), GLFW_KEY_S) == GLFW_PRESS) movement -= camera.direction;
    if(glfwGetKey(context.getWindow(), GLFW_KEY_D) == GLFW_PRESS) movement += right;
    if(glfwGetKey(context.getWindow(), GLFW_KEY_A) == GLFW_PRESS) movement -= right;
    movement *= deltaTime * camera.speed;
    camera.pos += movement;
    camera.lastTime = curTime;
}

void updateState(Context const &context, State &state){
    glfwPollEvents();
    if(glfwWindowShouldClose(context.getWindow()) != 0) state.doQuit = true;

    updateCamera(context, state);
    Triangle &t = state.get<Triangle>("triangle");
    t.rot += 0.01f;
    Cube &c = state.get<Cube>("cube");
    c.rot -= 0.01f;
}

void setCamera(State &state, Shaders &shaders){
    Camera &camera = state.get<Camera>("camera");
    state.view = glm::lookAt(
            camera.pos, // Camera pos.
            camera.pos + camera.direction, // Looking at.
            camera.up  // Up-direction.
    );

    for(auto &shader : shaders) {
        glUniformMatrix4fv(shader.second->v_matrix_id, 1, GL_FALSE, &state.view[0][0]);
        glUniformMatrix4fv(shader.second->p_matrix_id, 1, GL_FALSE, &state.projection[0][0]);
    }
}

void setLight(State &state, Shaders &shaders){
    //Camera &camera = state.get<Camera>("camera");
    static vec3 ligth_position = vec3{4.0f, 0.0f, 4.0f};
    //glUniform3fv(shader.lightposition_id, 1, &camera.pos[0]);
    for(auto &shader : shaders) {
        glUniform3fv(shader.second->lightposition_id, 1, &ligth_position[0]);
    }
}

void view(Context const& context, Model &model, State &state, Shaders &shaders){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto &shader : shaders) {
        setCamera(state, shaders);
        setLight(state, shaders);
        model.draw(context, state, shader.second->program_id);
    }
}

void updateShader(Shaders &shaders, State &state){
    if(state.updateShader) {
        loadShaders(shaders);
        state.updateShader = false;
    }
}

void initInput(InputHandler &inputHandler, State &state, Context &context){
    inputHandler.addHandler({GLFW_KEY_R, GLFW_PRESS, GLFW_MOD_CONTROL}, [&state](){state.updateShader = true;});
    inputHandler.addHandler({GLFW_KEY_ESCAPE, GLFW_PRESS, 0}, [&state](){state.doQuit = true;});
    inputHandler.addHandler({GLFW_KEY_Q, GLFW_PRESS, 0}, [&state, &context](){
        auto &camera = state.get<Camera>("camera");
        camera.captureMouse = !camera.captureMouse;
        if(camera.captureMouse) {
            glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(context.getWindow(), context.width / 2, context.height / 2);
        }
        else{
            glfwSetInputMode(context.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    });
}

void mainLoop(Context &context, Model &model, State &state, Shaders &shaders){
    for(;!state.doQuit;){
        updateState(context, state);
        view(context, model, state, shaders);
        glfwSwapBuffers(context.getWindow());
        updateShader(shaders, state);
    }
}

int main() {

    if(!glfwInit()){
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