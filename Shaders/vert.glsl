#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uvCoords;

layout (location=3) uniform mat4 pMat;
layout (location=4) uniform mat4 vMat;
layout (location=5) uniform mat4 mMat;

out VS_OUT
{   
    vec3 position;
    vec3 normal;
    vec2 uvCoords;
} vs_out;

void main(void)
{
    gl_Position = pMat*vMat*mMat*vec4(position, 1.0);
    vs_out.position = position; 
    vs_out.normal = normal;
    vs_out.uvCoords = uvCoords;
}
