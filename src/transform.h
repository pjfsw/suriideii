#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "algebra.h"

typedef struct {
    Vector3f position;
    Vector3f rotation;
    float scale;
    // Matrix should generally be read only, and (re)generated using transform_generate_matrix()
    Matrix4f m;
} Transform;

void transform_reset(Transform *transform);

void transform_rebuild(Transform *transform);


#endif