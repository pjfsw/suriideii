#version 420 core

in vec2 tex_coord_0;
out vec4 frag_color;

uniform sampler2D gSampler;

void main() {
    float depth = texture(gSampler, tex_coord_0).r;
    depth = 1.0 - (1.0 - depth) * 25.0;
    frag_color = vec4(depth, depth, depth, 1);
}