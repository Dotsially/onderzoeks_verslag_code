#version 460 core

layout (location = 0) in vec3 inPosition;

layout (location = 0) uniform mat4 projectionView;
layout (location = 1) uniform mat4 transform;

out vec3 outPosition;

void main(){
    outPosition = inPosition;
    gl_Position = projectionView * transform * vec4(inPosition, 1.0);
}