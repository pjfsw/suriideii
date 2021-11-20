#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "mesh.h"
#include "texture.h"
#include "transform.h"

typedef enum {
    COLLIDER_NONE,
    COLLIDER_SPHERE
} ColliderType;

typedef struct {
    float radius;
} SphereCollider;

typedef struct  {
    ColliderType type;
    SphereCollider sphere;
} Collider;

typedef struct {
    Transform transform;
    Collider collider;
} Geometry;

typedef struct {
    Mesh *mesh;
    Texture *texture;
    Geometry geometry;
    int type;
} Object;


Object *object_create(Mesh *mesh, Texture *texture, int type);

void geometry_set_collider_sphere(Geometry *geometry, float radius);

void object_destroy(Object *object);


#endif