#version 420 core

in vec2 tex_coord_0;
in vec3 normal_0;
out vec4 frag_color;

struct Light {
    vec3 direction;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
};

uniform sampler2D gSampler;
uniform Light gLight;

void main() {
    float diffuse_factor = dot(normalize(normal_0), -gLight.direction); 
    float diffuse_light = diffuse_factor * gLight.diffuse_intensity;
    frag_color =  vec4(gLight.color, 1) * (diffuse_light + gLight.ambient_intensity) * texture2D(gSampler, tex_coord_0);
}
