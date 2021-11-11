#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gTransformation;
uniform mat4 gWorld;
uniform mat4 gView;

out vec2 tex_coord_0;
out vec4 frag_color_0;

struct Light {
    vec3 direction;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
    float specular_power;
};
uniform Light gLight;

void main() {
    gl_Position = gTransformation * vec4(position, 1.0);
    tex_coord_0 = tex_coord;
    vec3 normal_0 = (gWorld * vec4(normal,0.0)).xyz;

    // Diffuse lighting component
    float diffuse_factor = dot(normalize(normal_0), -gLight.direction); 
    float diffuse_intensity = diffuse_factor * gLight.diffuse_intensity;

    // Specular lighting component
    vec3 eye_vector = -normalize((gView * vec4(position, 1.0)).xyz);
    vec3 reflect_vector_world = normalize(reflect(gLight.direction, normal));
    vec3 reflect_vector = normalize(vec3(gView * vec4(reflect_vector_world, 0.0)));

    float specular_factor = pow(max(dot(eye_vector, reflect_vector), 0.0), gLight.specular_power);
    float specular_intensity = gLight.specular_intensity * specular_factor;

    frag_color_0 = vec4(gLight.color, 1) * (specular_intensity + diffuse_intensity + gLight.ambient_intensity);
}
