//
// Created by aelg on 5/28/17.
//

#include <map>
#include "InputHandler.h"

class InputHandlerImpl : public InputHandler {
private:
    struct SortableKeyEvent : public KeyEvent{
        SortableKeyEvent(KeyEvent const &e) : KeyEvent(e){}
        bool operator<(SortableKeyEvent const &rhs) const {
            if(key != rhs.key) return key < rhs.key;
            if(action != rhs.action) return action < rhs.action;
            return mods < rhs.mods;
        }
    };
    std::map<SortableKeyEvent, Handler> m;
    GLFWwindow *window;
public:
    explicit InputHandlerImpl(GLFWwindow *window);
    ~InputHandlerImpl();
    void addHandler(KeyEvent const &e, Handler handler) override;
};

std::function<void(GLFWwindow *window, int key, int scancode, int action, int mods)> InputHandlerCallback;
static void callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    InputHandlerCallback(window, key, scancode, action, mods);
}


InputHandlerImpl::InputHandlerImpl(GLFWwindow *window) : m(), window(window) {
    InputHandlerCallback = [this](GLFWwindow *window, int key, int scancode, int action, int mods){
        auto it = this->m.find(SortableKeyEvent({key, action, mods}));
        if(it != this->m.end()){
            it->second();
        }
    };

    glfwSetKeyCallback(window, &callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

InputHandlerImpl::~InputHandlerImpl() {
    glfwSetKeyCallback(window, nullptr);
}

void InputHandlerImpl::addHandler(const InputHandler::KeyEvent &e, InputHandler::Handler handler) {
    m.insert({SortableKeyEvent(e),handler});
}

std::unique_ptr<InputHandler> InputHandler::makeInputHandler(GLFWwindow *window) {
    return std::unique_ptr<InputHandler>(new InputHandlerImpl(window));
}
