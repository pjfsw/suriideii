#version 420 core

in vec2 tex_coord_0;
in vec4 frag_color_0;
in vec4 shadow_coord_0;
out vec4 frag_color;

uniform sampler2D gSampler;
uniform sampler2D gShadowmap;

void main() {
    vec3 pshadow_coord = shadow_coord_0.xyz / shadow_coord_0.w;
    vec2 uv_coord;
    uv_coord.x = 0.5 * pshadow_coord.x + 0.5;
    uv_coord.y = 0.5 * pshadow_coord.y + 0.5;
    float z = 0.5 * pshadow_coord.z + 0.5;
    float visibility = 1.0;    
    float depth = texture(gShadowmap, uv_coord).x;

    if (depth  <  z + 0.0001){
        visibility = 0.9999;
    }
    frag_color = visibility * frag_color_0 * texture2D(gSampler, tex_coord_0);
}

