cmake_minimum_required(VERSION 3.8)
project(opengl)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DSHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

set(SOURCE_FILES
        main.cpp
        Context.cpp
        Context.h
        load_shaders.cpp
        load_shaders.h
        Model.cpp
        Model.h
        )
add_executable(opengl ${SOURCE_FILES})
target_link_libraries(opengl GL glfw GLEW)