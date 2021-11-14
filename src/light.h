#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "algebra.h"

typedef struct {
    Vector3f color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
    float specular_power;
} Light;

typedef struct {
    float constant;
    float linear;
    float exponential;
} Attenuation;

typedef struct {
    Light light;
    Vector3f direction;
} DirectionalLight;

typedef struct {
    Light light;
    Vector3f position;
    Attenuation attenuation;
} PointLight;

#endif