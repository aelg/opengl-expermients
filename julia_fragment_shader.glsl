#version 330 core

in vec3 fragmentColor;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;
in vec2 UV;
in float time;

out vec3 color;

void complex_square(in vec2 z, out vec2 o){
    o = vec2(z[0]*z[0] - z[1]*z[1], 2*z[0]*z[1]);
}

void main(){
    vec3 uvcolor;

    float r = fract(time*0.01)*0.5+0.4;
    float ra = time*0.5;
    //vec2 offset = vec2(2*fract(time*0.17438)-1, 2*fract(time*0.2758274)-1);
    vec2 offset = r*vec2(sin(ra), cos(ra));
    float zoom = 1.5;
    vec2 z = 2*zoom*UV - vec2(zoom, zoom);
    for(int i = 0; i < 20; ++i){
        complex_square(z + offset, z);
    }
    float c = sqrt(dot(z, z));
    uvcolor = clamp(vec3(z[0], z[1], z[0]*z[1]), 0, 1) + 0.1;
    /*if(UV[0] < middle+width && UV[0] > middle-width
    || UV[1] < middle+width && UV[1] > middle-width){
        uvcolor = vec3(0, 0.5, 0);
    }
    else {
        uvcolor = vec3(0.5, 0.5, 0.5);
    }*/
    color = uvcolor;// * (ambient + (diffuse+reflect) / distance);
}