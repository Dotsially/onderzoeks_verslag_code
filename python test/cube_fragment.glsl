#version 460 core

in vec3 outPosition;
out vec4 fragmentColor;

void main(){
    fragmentColor = vec4(outPosition, 1.0);
}