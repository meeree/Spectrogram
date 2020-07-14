#version 450 core

out vec4 color;

uniform vec3 lightIntensities = vec3(0.7);
uniform vec3 ambient = vec3(0.7);

uniform sampler2D sampler;
uniform sampler1D pallete;

layout (location=3) uniform mat4 pMat;
layout (location=4) uniform mat4 vMat;
layout (location=5) uniform mat4 mMat;

layout (location=6) uniform vec3 camPos;

layout (location=10) uniform int colorscheme; 

in VS_OUT
{   
    vec3 position;
    vec3 normal;
    vec2 uvCoords;
} fs_in;

void main(void)
{
	float t = texture(sampler, fs_in.uvCoords).r;

    if(colorscheme == 0)
    {
        float brk = 0.98;
        if(t > brk)
            color = vec4(1, 0, 0, 1);
        else 
            color = mix(vec4(1, 1, 1, 1), vec4(0.4, 0, 1, 1), t / brk);
    }
    else 
    {
        float brk = 0.98;
        if(t > brk)
            color = vec4(0, 0, 1, 1);
        else 
            color = mix(vec4(0, 0, 0, 1), vec4(0, 0.4, 1, 1), t / brk);
    }
}
