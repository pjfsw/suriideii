#version 420 core

struct Light {
    vec3 direction;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
    float specular_power;
};

in vec2 tex_coord_0;
in vec3 normal_0;
in vec4 world_position_0;
out vec4 frag_color;

uniform Light gLight;
uniform sampler2D gSampler;
uniform vec3 gCameraPos;

float get_diffuse_component(Light light) {
    float diffuse_factor = dot(normalize(normal_0), -light.direction); 
    return diffuse_factor * light.diffuse_intensity;
}

float get_specular_component(Light light) {
    vec3 reflected_light = normalize(reflect(-light.direction, normal_0));
    vec3 view_vector = normalize(world_position_0.xyz - gCameraPos);
    float specular_factor = dot(view_vector, reflected_light);
    float specular_intensity = 0;
    if (specular_factor > 0) {
        specular_factor = pow(specular_factor, light.specular_power);
        specular_intensity = light.specular_intensity * specular_factor;
    }
    return specular_intensity;
}

void main() {
    float diffuse_intensity = get_diffuse_component(gLight);
    float specular_intensity = get_specular_component(gLight);

    float intensity = specular_intensity + diffuse_intensity + gLight.ambient_intensity;
    frag_color = clamp(intensity, 0, 1) * vec4(gLight.color, 1) * texture2D(gSampler, tex_coord_0);
}

