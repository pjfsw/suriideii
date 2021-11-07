#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_color;

uniform mat4 gTransformation;

out vec4 color;

void main() {
    gl_Position = gTransformation * vec4(position, 1.0);
    color = vec4(in_color, 1.0f);
}
