#version 330 core

in vec3 fragmentColor;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

out vec3 color;

void main(){
    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    vec3 r = normalize(EyeDirection_cameraspace - 2*dot(EyeDirection_cameraspace, n)*n);
    //vec3 e = normalize(EyeDirection_cameraspace);
    float inFlux = clamp( dot( n,l ), 0, 1 );
    float reflection = clamp( dot( r,-l ), 0, 1 );
    float distance = dot(LightDirection_cameraspace, LightDirection_cameraspace);
    float ambient = 0.1;
    float diffuse = 1*20*inFlux;
    float reflect = 1*15*pow(reflection,100);
    color = fragmentColor * (ambient + (diffuse+reflect) / distance);
}