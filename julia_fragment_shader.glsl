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

    //float r = fract(time*0.01)*0.5+0.4;
    //float ra = time*0.5;
    //vec2 offset = r*vec2(sin(ra), cos(ra));
    float zoom = 1;
    vec2 z = 2*zoom*UV - vec2(zoom-0.5, zoom);
    vec2 zoom_offset = 2*zoom*offset - vec2(zoom, zoom);
    for(int i = 0; i < 1000; ++i){
        complex_square(z + zoom_offset, z);
    }
    float c = sqrt(dot(z, z));
    float power = 1;
    uvcolor = clamp(vec3(pow(z[0],power), pow(z[1],power), pow(z[0]*z[1],power)), 0, 1) + 0.1;
    color = uvcolor;
}