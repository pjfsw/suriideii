#include <stdlib.h>
#include "object.h"

Object *object_create(Mesh *mesh, Texture *texture, int type) {
    Object *object = calloc(1, sizeof(Object));
    object->mesh = mesh;
    object->texture = texture;
    object->type = type;
    return object;
}

void object_destroy(Object *object) {
    if (object == NULL) {
        return;
    }
    free(object);
}


