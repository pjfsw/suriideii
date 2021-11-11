#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "algebra.h"

typedef struct {
    Vector3f direction;
    Vector3f color;
    float intensity;
} Light;

#endif