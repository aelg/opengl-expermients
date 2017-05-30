#version 330 core

layout(location = 0) in vec3 pos_vertex;
layout(location = 3) in vec2 uv_vertex;

uniform mat4 MVP;
uniform float time_vertex;
uniform vec2 offset_vertex;

out vec2 UV;
out vec2 offset;
out float time;

void main(){
    gl_Position = MVP * vec4(pos_vertex,1);

    UV = uv_vertex;
    time = time_vertex;
    offset = offset_vertex;
}