#version 330 core

in vec3 fragmentColor;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;
in vec2 UV;
in vec2 offset;
in float time;

out vec3 color;

void complex_square(in vec2 z, out vec2 o){
    o = vec2(z[0]*z[0] - z[1]*z[1], 2*z[0]*z[1]);
}

void main(){
    vec3 uvcolor;

    /*float r = fract(time*0.01)*0.5+0.4;
    float ra = time*0.5;
    vec2 julia_offset = r*vec2(sin(ra), cos(ra));*/

    float zoom = 1;
    vec2 z = vec2(0, 0);
    vec2 julia_offset = 2*zoom*offset - vec2(zoom, zoom);
    vec2 offset = 2*zoom*UV - vec2(zoom, zoom);
    for(int i = 0; i < 1000; ++i){
        complex_square(z + offset, z);
    }
    float c = sqrt(dot(z, z));
    uvcolor = clamp(vec3(z[0], z[1], z[0]*z[1]), 0, 1) + 0.1;
    if(distance(offset, julia_offset) < 0.002){
        uvcolor = vec3(1, 0, 0);
    }
    color = uvcolor;
}