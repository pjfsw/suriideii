#include "object.h"

#include <string.h>
#include <stdlib.h>


void _object_randomize_vertex_color(Vertex *v) {
    v->color.x = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.y = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.z = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
}

Object *object_cube() {
    Object *object = calloc(1, sizeof(Object));
    object->vertex_count = 8;
    object->vertices = calloc(object->vertex_count, sizeof(Vertex));

    Vertex vertices[object->vertex_count];
    for (int i = 0; i < object->vertex_count; i++) {
        _object_randomize_vertex_color(&vertices[i]);
    }
    double size = 1.0;
    // Create coords for a square
    vector3f_set(&vertices[0].position, -size, size, -size);
    vector3f_set(&vertices[1].position, size, size, -size);
    vector3f_set(&vertices[2].position, size, -size, -size);
    vector3f_set(&vertices[3].position, -size, -size, -size);

    vector3f_set(&vertices[4].position, -size, size, size);
    vector3f_set(&vertices[5].position, size, size, size);
    vector3f_set(&vertices[6].position, size, -size, size);
    vector3f_set(&vertices[7].position, -size, -size, size);

    memcpy(object->vertices, vertices, sizeof(Vertex)*object->vertex_count);

    object->index_count = 36;
    object->indices = calloc(object->index_count, sizeof(int));

    // Create indices for a cube
    unsigned int indices[] = {
        0,1,2, // front
        0,2,3,

        5,4,6, // back
        4,7,6,

        0,4,5, // top
        0,5,1,

        6,7,3,// bottom
        2,6,3,

        1,5,6, // right
        1,6,2,

        4,0,3, // left
        4,3,7
    };
    memcpy(object->indices, indices, sizeof(int)*object->index_count);
    return object;
}

void object_destroy(Object *object) {
    if (object == NULL) {
        return;
    }
    if (object->indices != NULL) {
        free(object->indices);
    }
    if (object->vertices != NULL) {
        free(object->vertices);
    }
    free(object);
}


