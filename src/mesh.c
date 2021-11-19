#include "mesh.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

Mesh *mesh_quad(double x_offset, double y_offset, double width, double height, int x_subdivision, int y_subdivision) {
    if (x_subdivision < 1 || y_subdivision < 1) {
        fprintf(stderr, "Invalid number of subdivisions %d %d\n", x_subdivision, y_subdivision);
        return NULL;
    }
    Mesh *mesh = calloc(1, sizeof(Mesh));
    mesh->vertex_count = 4 * x_subdivision * y_subdivision;
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));

    mesh->index_count = 6 * x_subdivision * y_subdivision;
    mesh->indices = calloc(mesh->index_count, sizeof(int));

    Vertex vertices[mesh->vertex_count];

    // TODO optimize vertex count and index count

    double sub_width = width/(float)x_subdivision;
    double sub_height = height/(float)y_subdivision;

    // Create indices for a mesh
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    int vertex_offset = 0;
    int index_offset = 0;
    for (int x = 0; x < x_subdivision; x++) {
        for (int y = 0; y < y_subdivision; y++) {
            double x1 = x_offset + (float)x * sub_width;
            double y1 = y_offset + (float)y * sub_height;
            double x2 = x1 + sub_width;
            double y2 = y1 + sub_height;
            printf("x1=%f\n", x1);
            // front
            vector3f_set(&vertices[vertex_offset].position, x1, y2, 0);
            vector2f_set(&vertices[vertex_offset].texture, 0, 1);
            vector3f_set(&vertices[vertex_offset+1].position, x2, y2, 0);
            vector2f_set(&vertices[vertex_offset+1].texture, 1, 1);
            vector3f_set(&vertices[vertex_offset+2].position, x2, y1, 0);
            vector2f_set(&vertices[vertex_offset+2].texture, 1, 0);
            vector3f_set(&vertices[vertex_offset+3].position, x1, y1, 0);
            vector2f_set(&vertices[vertex_offset+3].texture, 0, 0);
            for (int i = 0; i < 4; i++) {
                vector3f_set(&vertices[vertex_offset+i].normal, 0,0,-1);
            }
            for (int i = 0; i < 6; i++) {
                mesh->indices[index_offset++] = indices[i]+vertex_offset;
            }
            vertex_offset += 4;
            
        }
    }
    memcpy(mesh->vertices, vertices, sizeof(Vertex) * mesh->vertex_count);
    for (int i = 0; i < mesh->vertex_count; i++) {
        printf("Vertex %d = (%f,%f,%f)\n", i, mesh->vertices[i].position.x, mesh->vertices[i].position.y, mesh->vertices[i].position.z);
    }
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

void mesh_render(Mesh *mesh) {
    MeshGL *gl = &mesh->gl;

    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    long size = sizeof(Vector3f);
    
    // Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);
    size += sizeof(Vector2f);

    // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);

    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);
}

void mesh_destroy(Mesh *mesh) {
    if (mesh == NULL) {
        return;
    }
    glDeleteBuffers(1, &mesh->gl.vbo);
    glDeleteBuffers(1, &mesh->gl.ibo);
    glDeleteVertexArrays(1, &mesh->gl.vao);
    if (mesh->indices != NULL) {
        free(mesh->indices);
    }
    if (mesh->vertices != NULL) {
        free(mesh->vertices);
    }
    free(mesh);
}


