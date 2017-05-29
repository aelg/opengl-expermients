#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 VP;

uniform mat4 M;

void main(){
    gl_Position = VP * M * vec4(vertexPosition_modelspace,1);
}