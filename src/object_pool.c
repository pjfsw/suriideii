#include <stdio.h>
#include <stdlib.h>

#include "object_pool.h"

ObjectPool *object_pool_create(Mesh *mesh, Texture *texture, int count, int type) {
    if (mesh == NULL || texture == NULL || count < 1) {
        fprintf(stderr, "Failed to create object pool\n");
        return NULL;
    }
    ObjectPool *pool = calloc(1, sizeof(ObjectPool));
    pool->mesh = mesh;
    pool->texture = texture;
    pool->object_count = count;
    pool->objects = calloc(count, sizeof(Object));
    for (int i = 0; i < count; i++) {
        pool->objects[i] = object_create(mesh, texture, type);
    }
    return pool;
}

void object_pool_destroy(ObjectPool *pool, bool destroy_mesh_and_texture) {
    if (pool == NULL) {
        return;
    }
    for (int i = 0; i < pool->object_count; i++) {
        object_destroy(pool->objects[i]);
    }
    if (destroy_mesh_and_texture) {
        if (pool->mesh != NULL) {
            mesh_destroy(pool->mesh);
        }
        if (pool->texture != NULL) {
            texture_destroy(pool->texture);
        }
    }
    free(pool);
}

void object_pool_foreach(ObjectPool *pool, ObjectPoolForEach for_each, void *user_data) {
    if (pool == NULL) {
        return;
    }
    for (int i = 0; i < pool->object_count; i++) {
        if (pool->objects[i] != NULL) {
            for_each(pool->objects[i], i, user_data);
        }
    }
}

