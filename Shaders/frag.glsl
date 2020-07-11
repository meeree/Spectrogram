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

in VS_OUT
{   
    vec3 position;
    vec3 normal;
    vec2 uvCoords;
} fs_in;

void main(void)
{
	float t = texture(sampler, fs_in.uvCoords).r;
    color = mix(vec4(1, 0, 0, 1), vec4(1, 1, 0, 1), clamp(t, 0.5, 1.0));
}
