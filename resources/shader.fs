#version 420 core

in vec2 tex_coord_0;
in vec4 frag_color_0;
out vec4 frag_color;

uniform sampler2D gSampler;

void main() {
    frag_color = frag_color_0 * texture2D(gSampler, tex_coord_0);
}

