cmake_minimum_required(VERSION 3.8)
project(opengl)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

add_executable(opengl
        main.cpp
        Context.cpp
        load_shaders.cpp
        Model.cpp
        InputHandler.cpp)
target_link_libraries(opengl GL glfw GLEW)