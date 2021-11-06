#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_color;

uniform mat4 gRotation;

out vec4 color;

const vec4 colors[3] = vec4[3] (vec4(1, 0, 0, 1), vec4(0, 1, 0, 1), vec4(0, 0, 1, 1));

void main() {
    gl_Position = gRotation * vec4(position, 1.0);
    color = vec4(in_color, 1.0f);
}
