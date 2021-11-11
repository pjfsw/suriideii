#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gTransformation;
uniform mat4 gWorld;

out vec2 tex_coord_0;
out vec3 normal_0;

void main() {
    gl_Position = gTransformation * vec4(position, 1.0);
    tex_coord_0 = tex_coord;
    normal_0 = (gWorld * vec4(normal,0.0)).xyz;
}
