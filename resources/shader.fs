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
in vec4 shadow_coord_0;
in vec3 normal_0;
in vec4 world_position_0;
out vec4 frag_color;

uniform Light gLight;
uniform sampler2D gSampler;
uniform sampler2D gShadowmap;
uniform vec3 gCameraPos;

float get_diffuse_component(Light light) {
    // Diffuse lighting component
    float diffuse_factor = dot(normalize(normal_0), -light.direction); 
    return diffuse_factor * light.diffuse_intensity;
}

float get_specular_component(Light light) {
    // Specular lighting component
    vec3 reflected_light = normalize(reflect(-gLight.direction, normal_0));
    vec3 view_vector = normalize(world_position_0.xyz - gCameraPos);
    float specular_factor = dot(view_vector, reflected_light);
    float specular_intensity = 0;
    if (specular_factor > 0) {
        specular_factor = pow(specular_factor, gLight.specular_power);
        specular_intensity = gLight.specular_intensity * specular_factor;
    }
    return specular_intensity;
}

float get_visibility() {
    vec3 pshadow_coord = shadow_coord_0.xyz / shadow_coord_0.w;
    vec2 uv_coord;
    uv_coord.x = 0.5 * pshadow_coord.x + 0.5;
    uv_coord.y = 0.5 * pshadow_coord.y + 0.5;
    float z = 0.5 * pshadow_coord.z + 0.5;
    float visibility = 1.0;    
    float depth = texture(gShadowmap, uv_coord).x;

    if (depth  <  z + 0.0001){
        visibility = 0.8;
    }
    return visibility;
}

void main() {
    float diffuse_intensity = get_diffuse_component(gLight);
    float specular_intensity = get_specular_component(gLight);
    float visibility = get_visibility();

    float intensity = visibility * (specular_intensity + diffuse_intensity) + gLight.ambient_intensity;
    frag_color = clamp(intensity, 0, 1) * vec4(gLight.color, 1) * texture2D(gSampler, tex_coord_0);
}

