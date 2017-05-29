//
// Created by aelg on 5/14/17.
//

#ifndef OPENGL_MODEL_H
#define OPENGL_MODEL_H


#include "Context.h"
#include "load_shaders.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <cstring>

class State {
    std::map<std::string, std::unique_ptr<char>> data;
public:
    glm::mat4 projection;
    glm::mat4 view;
    bool doQuit = false;
    bool updateShader = false;

    template <class T,
            class = typename std::enable_if<std::is_trivially_destructible<T>::value, T>::type>
    void add(std::string name,
             T &data){
        char *p = new char[sizeof(T)];
        std::memcpy(p, &data, sizeof(T));
        this->data[name] = std::move(std::unique_ptr<char>(p));
    }
    template <class T,
            class = typename std::enable_if<std::is_trivially_destructible<T>::value, T>::type>
    T& get(std::string const &name) const {
        auto it = data.find(name);
        if(it == data.end()) throw std::runtime_error("No data named " + name);
        return *reinterpret_cast<T*>(it->second.get());
    }
};

class Model {
    using draw_function = std::function<void(Context const& context, State const &state, GLuint program)>;
    std::map<std::string, draw_function> objects;
public:
    void draw(Context const &context, State const &state, GLuint program);
    void add(std::string name, draw_function draw);
};

struct Triangle {
    float x, y, rot;
};
void makeTriangle(Model &, State &, std::unique_ptr<Shader> &, std::string const &name, Triangle const &initial_state);

struct Cube {
    float x, y, rot;
};
void makeCube(Model &, State &, std::unique_ptr<Shader> &, std::string const &name, Cube const &initial_state);

struct Square {
    float x, y, width, height;
};
void makeSquare(Model &, State &, std::unique_ptr<Shader> &, std::string const &name, GLfloat size, Square const &initial_state);

#endif //OPENGL_MODEL_H
