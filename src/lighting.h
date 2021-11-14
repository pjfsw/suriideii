#ifndef _LIGHTING_H
#define _LIGHTING_H


#include <GL/glew.h>
#include <GL/gl.h>

#include "algebra.h"
#include "light.h"

#define NUMBER_OF_POINT_LIGHTS 8

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

typedef struct {
    GLuint shader_program;
    GLint shader_point_light_count;
    LightingDirectionalLight directional_light;
    LightingPointLight point_lights[NUMBER_OF_POINT_LIGHTS];
    int point_light_count;
} Lighting;

Lighting *lighting_create(GLuint shader_program);

DirectionalLight *lighting_create_directional(Lighting *lighting, float x, float y, float z, float red, float green, float blue);

PointLight *lighting_create_point(Lighting *lighting, float x, float y, float z, float red, float green, float blue);

void lighting_destroy(Lighting *lighting);

#endif