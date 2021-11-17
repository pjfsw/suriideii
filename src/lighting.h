#ifndef _LIGHTING_H
#define _LIGHTING_H


#include <GL/glew.h>
#include <GL/gl.h>

#include "algebra.h"
#include "light.h"

#define NUMBER_OF_POINT_LIGHTS 8

typedef struct _Lighting Lighting;

Lighting *lighting_create(GLuint shader_program);

void lighting_set_shadow_strength(Lighting *lighting, float strength);

void lighting_set_default_reflection(Lighting *lighting, float ambient, float diffuse, float specular, float specular_power);

void lighting_set_default_attenuation(Lighting *lighting, float constant, float linear, float exponential);

DirectionalLight *lighting_create_directional(Lighting *lighting, float x, float y, float z, float red, float green, float blue);

PointLight *lighting_create_point(Lighting *lighting, float x, float y, float z, float red, float green, float blue);

void lighting_destroy(Lighting *lighting);

#endif