//
// Created by aelg on 5/28/17.
//

#ifndef OPENGL_INPUTHANDLER_H
#define OPENGL_INPUTHANDLER_H

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

class InputHandler {
private:

public:
    struct KeyEvent{
        int key;
        int action;
        int mods;
    };
    using Handler = std::function<void()>;
    explicit InputHandler() = default;
    virtual ~InputHandler() = default;

    virtual void addHandler(KeyEvent const &e, Handler handler) = 0;

    static std::unique_ptr<InputHandler> makeInputHandler(GLFWwindow *window);
};


#endif //OPENGL_INPUTHANDLER_H
