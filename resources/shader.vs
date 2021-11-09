#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;

uniform mat4 gTransformation;

out vec2 tex_coord_0;

void main() {
    gl_Position = gTransformation * vec4(position, 1.0);
    tex_coord_0 = tex_coord;
}
