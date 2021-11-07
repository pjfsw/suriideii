#include "mesh.h"

#include <string.h>
#include <stdlib.h>


void _mesh_randomize_vertex_color(Vertex *v) {
    v->color.x = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.y = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.z = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
}

Mesh *mesh_cube() {
    Mesh *mesh = calloc(1, sizeof(Mesh));
    mesh->vertex_count = 8;
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));

    Vertex vertices[mesh->vertex_count];
    for (int i = 0; i < mesh->vertex_count; i++) {
        _mesh_randomize_vertex_color(&vertices[i]);
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

    memcpy(mesh->vertices, vertices, sizeof(Vertex)*mesh->vertex_count);

    mesh->index_count = 36;
    mesh->indices = calloc(mesh->index_count, sizeof(int));

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
    memcpy(mesh->indices, indices, sizeof(int)*mesh->index_count);
    return mesh;
}

void mesh_destroy(Mesh *mesh) {
    if (mesh == NULL) {
        return;
    }
    if (mesh->indices != NULL) {
        free(mesh->indices);
    }
    if (mesh->vertices != NULL) {
        free(mesh->vertices);
    }
    free(mesh);
}


