#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gProjection;
uniform mat4 gCamera;
uniform mat4 gWorld;

void main() {
    gl_Position = gProjection * gCamera * gWorld * vec4(position, 1.0);
}