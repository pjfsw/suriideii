#version 420 core

struct Light {
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
    float specular_power;
};

struct Attenuation {
    float constant;
    float linear;
    float exponential;
};

struct DirectionalLight {
    Light light;
    vec3 direction;
};

struct PointLight {
    Light light;
    vec3 position;
    Attenuation attenuation;
};

in vec2 tex_coord_0;
in vec3 normal_0;
in vec4 world_position_0;
out vec4 frag_color;

uniform DirectionalLight gLight;
uniform PointLight gPointLight;

uniform sampler2D gSampler;
uniform vec3 gCameraPos;

float get_diffuse_component(Light light, vec3 direction) {
    float diffuse_factor = dot(normalize(normal_0), -direction); 
    return diffuse_factor * light.diffuse_intensity;
}

float get_specular_component(Light light, vec3 direction) {
    vec3 reflected_light = normalize(reflect(-direction, normal_0));
    vec3 view_vector = normalize(world_position_0.xyz - gCameraPos);
    float specular_factor = dot(view_vector, reflected_light);
    float specular_intensity = 0;
    if (specular_factor > 0) {
        specular_factor = pow(specular_factor, light.specular_power);
        specular_intensity = light.specular_intensity * specular_factor;
    }
    return specular_intensity;
}

vec4 get_light(Light light, vec3 direction) {
    float diffuse_intensity = get_diffuse_component(light, direction);
    float specular_intensity = get_specular_component(light, direction);

    float intensity = specular_intensity + diffuse_intensity + light.ambient_intensity;
    return clamp(intensity, 0, 1) * vec4(light.color, 1);
}

vec4 get_directional_light(DirectionalLight light) {
    return get_light(light.light, light.direction);
}

vec4 get_point_light(PointLight light) {
    vec3 direction = (world_position_0 - vec4(light.position, 1)).xyz;
    float distance = length(direction);
    direction = normalize(direction);    
    vec4 color = get_light(light.light, direction);
    float attenuation = 
        light.attenuation.constant +
        light.attenuation.linear * distance +
        light.attenuation.exponential * distance * distance;
    return color / attenuation;
}

void main() {
    vec4 directional_light = get_directional_light(gLight);
    vec4 point_light = get_point_light(gPointLight);
    vec4 total_light = directional_light + point_light;

    frag_color = total_light * texture2D(gSampler, tex_coord_0);
}

