#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gPerspective;
uniform mat4 gWorld;
uniform mat4 gCamera;
uniform mat4 gLightTransform;

out vec2 tex_coord_0;
out vec4 shadow_coord_0;
out vec3 normal_0;
out vec4 world_position_0;

void main() {
    world_position_0 = gWorld * vec4(position, 1.0);
    gl_Position = gPerspective * gCamera * world_position_0;
    shadow_coord_0 = gPerspective * gLightTransform * vec4(world_position_0);
    
    normal_0 = normalize((gWorld * vec4(normal,0.0)).xyz);
    tex_coord_0 = tex_coord;
}
