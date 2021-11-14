#version 420 core

layout (location = 0) in vec3 position;

uniform mat4 gLightTransform;
uniform mat4 gWorld;

void main() {
    gl_Position = gLightTransform * gWorld * vec4(position, 1.0);
}