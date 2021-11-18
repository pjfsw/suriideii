#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "mesh.h"
#include "texture.h"
#include "transform.h"

typedef struct {
    Mesh *mesh;
    Texture *texture;
    Transform transform;
    int type;
} Object;

Object *object_create(Mesh *mesh, Texture *texture, int type);

void object_destroy(Object *object);


#endif