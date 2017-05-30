cmake_minimum_required(VERSION 3.8)
project(opengl)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders\")

add_definitions(-g)

add_executable(opengl
        main.cc
        Context.cc
        load_shaders.cc
        Model.cc
        InputHandler.cc)
target_link_libraries(opengl GL glfw GLEW)

add_executable(mandelbrot
        mandelbrot.cc
        Context.cc
        load_shaders.cc
        Model.cc
        InputHandler.cc)
target_link_libraries(mandelbrot GL glfw GLEW)
