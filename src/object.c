#include <stdlib.h>
#include "object.h"


Object *object_create(Mesh *mesh, Texture *texture) {
    Object *object = calloc(1, sizeof(Object));
    object->mesh = mesh;
    object->texture = texture;
    return object;
}

void object_destroy(Object *object) {
    if (object == NULL) {
        return;
    }
    free(object);
}


