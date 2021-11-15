#include "mesh.h"

#include <string.h>
#include <stdlib.h>

Mesh *mesh_cube() {
    Mesh *mesh = calloc(1, sizeof(Mesh));
    mesh->vertex_count = 24;
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));

    Vertex vertices[mesh->vertex_count];

    double size = 1.0;
    // front
    vector3f_set(&vertices[0].position, -size, size, -size);
    vector2f_set(&vertices[0].texture, 0, 0);
    vector3f_set(&vertices[1].position, size, size, -size);
    vector2f_set(&vertices[1].texture, 1, 0);
    vector3f_set(&vertices[2].position, size, -size, -size);
    vector2f_set(&vertices[2].texture, 1, 1);
    vector3f_set(&vertices[3].position, -size, -size, -size);
    vector2f_set(&vertices[3].texture, 0, 1);
    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i].normal, 0,0,-1);
    }

    // back
    vector3f_set(&vertices[4].position, -size, size, size);
    vector2f_set(&vertices[4].texture, 0, 1);
    vector3f_set(&vertices[5].position, size, size, size);
    vector2f_set(&vertices[5].texture, 1, 1);
    vector3f_set(&vertices[6].position, size, -size, size);
    vector2f_set(&vertices[6].texture, 1, 0);
    vector3f_set(&vertices[7].position, -size, -size, size);
    vector2f_set(&vertices[7].texture, 0, 0);
    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i+4].normal, 0,0,1);
    }


    // top
    vector3f_set(&vertices[8].position, -size, size, -size);
    vector2f_set(&vertices[8].texture, 0, 0);
    vector3f_set(&vertices[9].position, -size, size, size);
    vector2f_set(&vertices[9].texture, 1, 0);
    vector3f_set(&vertices[10].position, size, size, size);
    vector2f_set(&vertices[10].texture, 1, 1);
    vector3f_set(&vertices[11].position, size, size, -size);
    vector2f_set(&vertices[11].texture, 0, 1);

    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i+8].normal, 0,1,0);
    }

    // bottom
    vector3f_set(&vertices[12].position, -size, -size, -size);
    vector2f_set(&vertices[12].texture, 0, 0);
    vector3f_set(&vertices[13].position, size, -size, -size);
    vector2f_set(&vertices[13].texture, 1, 0);
    vector3f_set(&vertices[14].position, size, -size, size);
    vector2f_set(&vertices[14].texture, 1, 1);
    vector3f_set(&vertices[15].position, -size, -size, size);
    vector2f_set(&vertices[15].texture, 0, 1);

    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i+12].normal, 0,-1,0);
    }

    // left
    vector3f_set(&vertices[16].position, -size, size, size);
    vector2f_set(&vertices[16].texture, 0, 0);
    vector3f_set(&vertices[17].position, -size, size, -size);
    vector2f_set(&vertices[17].texture, 1, 0);
    vector3f_set(&vertices[18].position, -size, -size, -size);
    vector2f_set(&vertices[18].texture, 1, 1);
    vector3f_set(&vertices[19].position, -size, -size, size);
    vector2f_set(&vertices[19].texture, 0, 1);

    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i+16].normal, -1, 0,0);
    }

    // right
    vector3f_set(&vertices[20].position, size, size, -size);
    vector2f_set(&vertices[20].texture, 0, 0);
    vector3f_set(&vertices[21].position, size, size, size);
    vector2f_set(&vertices[21].texture, 1, 0);
    vector3f_set(&vertices[22].position, size, -size, size);
    vector2f_set(&vertices[22].texture, 1, 1);
    vector3f_set(&vertices[23].position, size, -size, -size);
    vector2f_set(&vertices[23].texture, 0, 1);

    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i+20].normal, 1, 0,0);
    }

    memcpy(mesh->vertices, vertices, sizeof(Vertex)*mesh->vertex_count);

    mesh->index_count = 36;
    mesh->indices = calloc(mesh->index_count, sizeof(int));

    // Create indices for a cube
    unsigned int indices[] = {
        0,1,2, // front
        0,2,3,

        5,4,6, // back
        4,7,6,

        8,9,10, // top
        8,10,11,

        12,13,14, // bottom
        12,14,15,

        16,17,18, // left
        16,18,19,

        20,21,22, // right
        20,22,23
    };

    memcpy(mesh->indices, indices, sizeof(int)*mesh->index_count);
    return mesh;
}

Mesh *mesh_quad() {
    Mesh *mesh = calloc(1, sizeof(Mesh));
    mesh->vertex_count = 4;
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));

    Vertex vertices[mesh->vertex_count];

    double size = 1.0;
    // front
    vector3f_set(&vertices[0].position, -size, size, 0);
    vector2f_set(&vertices[0].texture, 0, 0);
    vector3f_set(&vertices[1].position, size, size, 0);
    vector2f_set(&vertices[1].texture, 1, 0);
    vector3f_set(&vertices[2].position, size, -size, 0);
    vector2f_set(&vertices[2].texture, 1, 1);
    vector3f_set(&vertices[3].position, -size, -size, 0);
    vector2f_set(&vertices[3].texture, 0, 1);
    for (int i = 0; i < 4; i++) {
        vector3f_set(&vertices[i].normal, 0,0,-1);
    }

    memcpy(mesh->vertices, vertices, sizeof(Vertex)*mesh->vertex_count);

    mesh->index_count = 6;
    mesh->indices = calloc(mesh->index_count, sizeof(int));

    // Create indices for a cube
    unsigned int indices[] = {
        0,1,2, // front
        0,2,3
    };

    memcpy(mesh->indices, indices, sizeof(int)*mesh->index_count);
    return mesh;
}

void mesh_instantiate(Mesh *mesh) {
    GLuint *vbo = &mesh->gl.vbo;
    GLuint *vao = &mesh->gl.vao;
    GLuint *ibo = &mesh->gl.ibo;

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);    
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count*sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, ibo);
    // TODO vertex array?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count*sizeof(int), mesh->indices, GL_STATIC_DRAW);
}

void mesh_destroy(Mesh *mesh) {
    if (mesh == NULL) {
        return;
    }
    glDeleteBuffers(1, &mesh->gl.vbo);
    if (mesh->indices != NULL) {
        free(mesh->indices);
    }
    if (mesh->vertices != NULL) {
        free(mesh->vertices);
    }
    free(mesh);
}


