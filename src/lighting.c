#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lighting.h"
#include "uniform.h"

typedef struct {
    GLint color;
    GLint ambient_intensity;
    GLint diffuse_intensity; 
    GLint specular_intensity;
    GLint specular_power;
} ShaderLight;

typedef struct {
    ShaderLight light;
    GLint direction;
} ShaderDirectionalLight;

typedef struct {
    GLint constant;
    GLint linear;
    GLint exponential;
} ShaderAttenuation;

typedef struct {
    ShaderLight light;
    GLint position;
    ShaderAttenuation attenuation;    
} ShaderPointLight;

typedef struct {
    ShaderPointLight shader;
    PointLight *light;
} LightingPointLight;

typedef struct {
    ShaderDirectionalLight shader;
    DirectionalLight *light;
} LightingDirectionalLight;

struct _Lighting {
    Light default_reflection;
    Attenuation default_attenuation;
    GLuint shader_program;
    GLint shader_point_light_count;
    LightingDirectionalLight directional_light;
    LightingPointLight point_lights[NUMBER_OF_POINT_LIGHTS];
    int point_light_count;
};

void _lighting_setup_light(Light *light, ShaderLight *sl) {
    glUniform3f(sl->color, light->color.x, light->color.y, light->color.z);
    glUniform1f(sl->ambient_intensity, light->ambient_intensity);
    glUniform1f(sl->diffuse_intensity, light->diffuse_intensity);
    glUniform1f(sl->specular_intensity, light->specular_intensity);
    glUniform1f(sl->specular_power, light->specular_power);
}

void _lighting_setup_directional_light(DirectionalLight *light, ShaderDirectionalLight *sl) {
    _lighting_setup_light(&light->light, &sl->light);
    glUniform3f(sl->direction, light->direction.x, light->direction.y,
        light->direction.z);   
}

void _lighting_setup_point_light(PointLight *light, ShaderPointLight *sl) {
    _lighting_setup_light(&light->light, &sl->light);
    glUniform3f(sl->position, light->position.x, light->position.y, light->position.z);
    glUniform1f(sl->attenuation.constant, light->attenuation.constant);
    glUniform1f(sl->attenuation.linear, light->attenuation.linear);
    glUniform1f(sl->attenuation.exponential, light->attenuation.exponential);
}

bool _lighting_init_shader_light(GLuint program, char *prefix, ShaderLight *light) {
    glUseProgram(program);
    char var[100];
    strcpy(var, prefix);
    strcat(var, ".color");
    light->color = glGetUniformLocation(program, var);
    if (light->color < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".diffuse_intensity");
    light->diffuse_intensity = glGetUniformLocation(program, var);
    if (light->diffuse_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".ambient_intensity");
    light->ambient_intensity = glGetUniformLocation(program, var);
    if (light->ambient_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".specular_intensity");
    light->specular_intensity = glGetUniformLocation(program, var);
    if (light->specular_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".specular_power");
    light->specular_power = glGetUniformLocation(program, var);
    if (light->specular_power < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }

    return true;
}

bool _lighting_init_shader_directional_light(GLuint program, char *prefix, ShaderDirectionalLight *light) {
    char var[100];
    strcpy(var, prefix);
    strcat(var, ".direction");
    
    if (!uniform_assign(program, &light->direction, var)) {
        return false;
    }
    strcpy(var, prefix);
    strcat(var, ".light");
    return _lighting_init_shader_light(program, var, &light->light);
}

bool _lighting_init_shader_point_light(GLuint program, char *prefix, ShaderPointLight *light) {
    char var[100];
    strcpy(var, prefix);
    strcat(var, ".position");

    if (!uniform_assign(program, &light->position, var)) {
        return false;
    }

    strcpy(var, prefix);
    strcat(var, ".attenuation.constant");
    if (!uniform_assign(program, &light->attenuation.constant, var)) {
        return false;
    }

    strcpy(var, prefix);
    strcat(var, ".attenuation.linear");
    if (!uniform_assign(program, &light->attenuation.linear, var)) {
        return false;
    }

    strcpy(var, prefix);
    strcat(var, ".attenuation.exponential");
    if (!uniform_assign(program, &light->attenuation.exponential, var)) {
        return false;
    }

    strcpy(var, prefix);
    strcat(var, ".light");
    return _lighting_init_shader_light(program, var, &light->light);

}


Lighting *lighting_create(GLuint shader_program) {
    Lighting *lighting = calloc(1, sizeof(Lighting));
    lighting->shader_program = shader_program;
    if (!uniform_assign(lighting->shader_program, &lighting->shader_point_light_count, "gPointLightCount")) {
        return false;
    }
    lighting_set_default_reflection(lighting, 0.45, 0.3, 0.2, 32);
    lighting_set_default_attenuation(lighting, 0.01, 0.03, 0.02);

    return lighting;
}

void lighting_set_default_reflection(Lighting *lighting, float ambient, float diffuse, float specular, float specular_power) {
    lighting->default_reflection.ambient_intensity = ambient;
    lighting->default_reflection.diffuse_intensity = diffuse;
    lighting->default_reflection.specular_intensity = specular;
    lighting->default_reflection.specular_power = specular_power;
}

void lighting_set_default_attenuation(Lighting *lighting, float constant, float linear, float exponential) {
    lighting->default_attenuation.constant = constant;
    lighting->default_attenuation.linear = linear;
    lighting->default_attenuation.exponential = exponential;
}

DirectionalLight *lighting_create_directional(Lighting *lighting, float x, float y, float z, float red, float green, float blue) {
    if (lighting == NULL || lighting->directional_light.light != NULL) {
        fprintf(stderr, "Failed to create directional light\n");
        return NULL;
    }
    if (!_lighting_init_shader_directional_light(lighting->shader_program, "gLight", &lighting->directional_light.shader)) {
        return NULL;
    }
    DirectionalLight *light = calloc(1, sizeof(DirectionalLight));
    vector3f_set_and_normalize(&light->direction, x, y, z);
    memcpy(&light->light, &lighting->default_reflection, sizeof(Light));
    vector3f_set(&light->light.color, red, green, blue);
    _lighting_setup_directional_light(light, &lighting->directional_light.shader);
    lighting->directional_light.light = light;
    return light;
}

PointLight *lighting_create_point(Lighting *lighting, float x, float y, float z, float red, float green, float blue) {
    if (lighting == NULL || lighting->point_light_count >= NUMBER_OF_POINT_LIGHTS) {
        fprintf(stderr, "Failed to create point light\n");
        return NULL;
    }
    char s[100];
    sprintf(s, "gPointLight[%d]", lighting->point_light_count);
    LightingPointLight *point_light = &lighting->point_lights[lighting->point_light_count];
    if (!_lighting_init_shader_point_light(lighting->shader_program, s, &point_light->shader)) {
        return false;
    }
    PointLight *light = calloc(1, sizeof(PointLight));
    point_light->light = light;
    vector3f_set(&light->position, x, y, z);
    memcpy(&light->attenuation, &lighting->default_attenuation, sizeof(Attenuation));
    memcpy(&light->light, &lighting->default_reflection, sizeof(Light));
    vector3f_set(&light->light.color, red, green, blue);
    _lighting_setup_point_light(light, &point_light->shader);
    point_light->light = light;
    lighting->point_light_count++;
    glUniform1i(lighting->shader_point_light_count, lighting->point_light_count);
    return light;
}


void lighting_destroy(Lighting *lighting) {
    if (lighting == NULL) {
        return;
    }
    if (lighting->directional_light.light != NULL) {
        free(lighting->directional_light.light);
    }
    for (int i = 0 ; i < lighting->point_light_count; i++) {
        if (lighting->point_lights[i].light != NULL) {
            free(lighting->point_lights[i].light);
        }
    }
    free(lighting);
}