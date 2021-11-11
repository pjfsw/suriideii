#version 420 core

in vec2 tex_coord_0;
in vec3 normal_0;
out vec4 frag_color;

uniform sampler2D gSampler;
uniform vec3 gDiffuseLight;

void main() {
    float diffuse_factor = dot(normalize(normal_0), -gDiffuseLight); 
    frag_color = diffuse_factor * texture2D(gSampler, tex_coord_0);
}
