#version 420 core

in vec2 tex_coord_0;
in vec4 frag_color_0;
in vec4 shadow_coord_0;
out vec4 frag_color;

uniform sampler2D gSampler;
uniform sampler2D gShadowmap;

void main() {
    float visibility = 1.0;
    if (texture(gShadowmap, shadow_coord_0.xy).z  <  shadow_coord_0.z - 0.2){
        visibility = 0.4;
    }
    frag_color = visibility * frag_color_0 * texture2D(gSampler, tex_coord_0);
}

