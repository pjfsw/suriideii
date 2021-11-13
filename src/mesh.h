#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>
#include <GL/gl.h>

#include "algebra.h"

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
} MeshGL;

typedef struct {
    Vector3f position;
    Vector2f texture;
    Vector3f normal;
} Vertex;

typedef struct {    
    Vertex *vertices;
    int vertex_count;        
    int *indices;
    int index_count;
    MeshGL gl;
} Mesh;

Mesh *mesh_cube();

void mesh_instantiate(Mesh *mesh);

void mesh_destroy(Mesh *mesh);

#endif