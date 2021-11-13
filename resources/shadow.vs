#version 420 core

layout (location = 0) in vec3 position;

uniform mat4 gLightMatrix;
uniform mat4 gWorld;

void main() {
    gl_Position = gLightMatrix * gWorld * vec4(position, 1.0);
}