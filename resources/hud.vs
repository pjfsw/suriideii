#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gWorld;
uniform float gAspectRatio;
out vec2 tex_coord_0;

void main() {
    gl_Position = gWorld *  vec4(position.x * gAspectRatio, position.y, -1, 1.0);
    tex_coord_0 = tex_coord;
}