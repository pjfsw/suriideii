#version 420 core

in vec2 tex_coord_0;
in vec3 normal_0;
out vec4 frag_color;

struct Light {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform sampler2D gSampler;
uniform Light gAmbientLight;
uniform DirectionalLight gDiffuseLight;

void main() {
    float diffuse_factor = dot(normalize(normal_0), -gDiffuseLight.direction); 
    vec4 diffuse_light = diffuse_factor * gDiffuseLight.intensity * vec4(gDiffuseLight.color, 1);
    vec4 ambient_light = gAmbientLight.intensity * vec4(gAmbientLight.color, 1);
    frag_color = (diffuse_light + ambient_light) * texture2D(gSampler, tex_coord_0);
}
