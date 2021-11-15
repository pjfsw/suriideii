#version 420 core

in vec2 tex_coord_0;
out vec4 frag_color;

uniform sampler2D gSampler;

void main() {
    frag_color = vec4(texture2D(gSampler, tex_coord_0).xyz, 1.0);
}