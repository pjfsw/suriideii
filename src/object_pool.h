#ifndef _OBJECT_POOL_H
#define _OBJECT_POOL_H

#include "mesh.h"
#include "texture.h"
#include "object.h"

#include <stdbool.h>

typedef struct {
    Mesh *mesh;
    Texture *texture;
    Object **objects;
    int object_count;
} ObjectPool;

typedef void (*ObjectPoolForEach)(Object *object, int index, void *user_data);

ObjectPool *object_pool_create(Mesh *mesh, Texture *texture, int count, int type);

void object_pool_destroy(ObjectPool *pool, bool destroy_mesh_and_texture);

void object_pool_foreach(ObjectPool *pool, ObjectPoolForEach for_each, void *user_data);

#endif