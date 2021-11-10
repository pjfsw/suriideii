#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "meshloader.h"

typedef struct {
    Mesh *mesh;
    int vertex;
    int index;
} Meshloader;

int _extract_vertex_index(char *s) {
    char *slash = strchr(s, '/');
    if (slash == NULL) {
        return atoi(s);
    } 
    int bufsize = 64;
    char buf[bufsize];
    strncpy(buf, s, bufsize);
    return atoi(buf);
}

void _mesh_loader_count(Meshloader *meshloader, char *buf) {
    Mesh *mesh = meshloader->mesh;

    char s[4][100];

    float x;
    float y;
    float z;
    float w = 1;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        mesh->vertex_count++;
    }
    int index_count = sscanf(buf, "f %s %s %s %s", s[0], s[1], s[2], s[3]);
    if (index_count == 3) {
        mesh->index_count += 3;
    } else if (index_count == 4) {
        mesh->index_count += 6;
    }
}

void _mesh_loader_populate(Meshloader *meshloader, char *buf) {
    Mesh *mesh = meshloader->mesh;
    char s[4][100];

    float x;
    float y;
    float z;
    float w = 1;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        Vertex *v = &mesh->vertices[meshloader->vertex++];
        v->position.x = x;
        v->position.y = y;
        v->position.z = z;
        v->texture.x = (float)(meshloader->vertex % 3)/3.0;
        v->texture.y = (float)((meshloader->vertex / 3) % 3)/3.0;
    }

    int index_count = sscanf(buf, "f %s %s %s %s", s[0], s[1], s[2], s[3]);
    int index[4];
    for (int i = 0; i < index_count; i++) {
        index[i] = _extract_vertex_index(s[i]);
    }
    if (index_count == 3) {
        mesh->indices[meshloader->index++] = index[0]-1;
        mesh->indices[meshloader->index++] = index[1]-1;
        mesh->indices[meshloader->index++] = index[2]-1;
    } else if (index_count == 4) {
        mesh->indices[meshloader->index++] = index[0]-1;
        mesh->indices[meshloader->index++] = index[1]-1;
        mesh->indices[meshloader->index++] = index[2]-1;

        mesh->indices[meshloader->index++] = index[0]-1;
        mesh->indices[meshloader->index++] = index[2]-1;
        mesh->indices[meshloader->index++] = index[3]-1;
    }

}

Mesh *mesh_loader_load(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open mesh file '%s'\n", file_name);
        return NULL;
    }
    int bufsize = 256;

    char buf[bufsize];
    Mesh *mesh = calloc(1, sizeof(Mesh));   
    Meshloader meshloader;
    memset(&meshloader, 0, sizeof(Meshloader));
    meshloader.mesh = mesh;

    while (!feof(fp)) {        
        fgets(buf, bufsize, fp);
        _mesh_loader_count(&meshloader, buf);
    }
    printf("Detected %d vertices and %d indices\n", mesh->vertex_count, mesh->index_count);
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));
    mesh->indices = calloc(mesh->index_count, sizeof(int));
    fseek(fp, 0L, SEEK_SET);    
    while (!feof(fp)) {
        fgets(buf, bufsize, fp);
        _mesh_loader_populate(&meshloader, buf);
    }
    fclose(fp);
    return mesh;

}
