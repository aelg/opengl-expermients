#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform mat4 VP;
uniform mat4 M;

out vec3 fragmentColor;

void main(){
    gl_Position = VP * M * vec4(vertexPosition,1);
    fragmentColor = vertexColor;
}