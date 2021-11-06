#version 420 core

layout (location = 0) in vec3 position;

uniform mat4 gRotation;

out vec4 color;

const vec4 colors[3] = vec4[3] (vec4(1, 0, 0, 1), vec4(0, 1, 0, 1), vec4(0, 0, 1, 1));

void main() {
    gl_Position = gRotation * vec4(position, 1.0);
    color = colors[gl_VertexID];
}
