#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct {
    Vector3f position;
    Vector2f texture;
} Vertex;

typedef struct {    
    Vertex *vertices;
    int vertex_count;        
    int *indices;
    int index_count;
} Mesh;

Mesh *mesh_cube();

void mesh_destroy(Mesh *mesh);

#endif