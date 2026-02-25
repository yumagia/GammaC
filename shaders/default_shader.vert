#version 450 core

layout(std140, binding = 0) uniform Matrices {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 modelViewMatrix;
    mat4 projectionMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 lightmapUv;

out vec4 vertexColor;
out vec2 texCoord;

void main() 
{
    texCoord = lightmapUv.xy;
    vertexColor = vec4(1, 0.5, 0.7, 1);
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1);
}