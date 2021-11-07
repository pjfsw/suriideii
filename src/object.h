#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "algebra.h"

typedef struct {
    Vector3f position;
    Vector3f color;
} Vertex;

typedef struct {    
    Vertex *vertices;
    int vertex_count;        
    int *indices;
    int index_count;
} Object;

Object *object_cube();

void object_destroy(Object *object);

#endif