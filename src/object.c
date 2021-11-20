#include <stdlib.h>
#include "object.h"

Object *object_create(Mesh *mesh, Texture *texture, int type) {
    Object *object = calloc(1, sizeof(Object));
    object->mesh = mesh;
    object->texture = texture;
    object->type = type;
    object->geometry.collider.type = COLLIDER_NONE;
    return object;
}

void object_destroy(Object *object) {
    if (object == NULL) {
        return;
    }
    free(object);
}

void geometry_set_collider_sphere(Geometry *geometry, float radius) {
    geometry->collider.type = COLLIDER_SPHERE;
    geometry->collider.sphere.radius = radius;
}
