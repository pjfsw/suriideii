#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "meshloader.h"

typedef struct {
    Mesh *mesh;
    int vertex;
    int index;
    int texture;
    int texture_count;
    int texture_overwrites;
    Vector2f *textures;
    bool *vertex_writes;
    Vector3f *normals;
    int normal_count;
    int normal_overwrites;
    int normal;

} Meshloader;

typedef struct {
    int v;
    int t;
    int n;
} FaceData;

void _extract_face_data(char *s, FaceData *face) {
    //printf(" Extract from %s: ", s);
    memset(face, 0, sizeof(FaceData));
    char *slash = strchr(s, '/');
    if (slash == NULL) {
        face->v = atoi(s);
        return;
    } 
    int bufsize = 64;
    char buf[bufsize];
    memset(buf,0,bufsize);

    int len = slash-s;
    strncpy(buf, s, len);    
    face->v = atoi(buf);
    //printf("%s,%s,%s=%d ", s, slash, buf, face->v);
    
    slash++;
    char *slash2 = strchr(slash, '/');
    if (slash2 == NULL) {
        face->t = atoi(slash);
        return;
    }

    memset(buf,0,bufsize);
    strncpy(buf, slash, slash2-slash);
    face->t = atoi(buf);

    slash2++;
    face->n = atoi(slash2);

//    printf(" (%d,%d,%d)\n", face->v, face->t, face->n);
}

void _mesh_loader_count(Meshloader *meshloader, char *buf) {
    Mesh *mesh = meshloader->mesh;

    char s[4][100];

    float x;
    float y;
    float z;
    float u;
    float v; 
    float t;
    float w = 1;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        mesh->vertex_count++;
    }
    if (sscanf(buf, "vt %f %f %f", &u,&v,&t) > 1) {
        meshloader->texture_count++;
    }
    if (sscanf(buf, "vn %f %f %f", &x, &y, &z) > 2) {
        meshloader->normal_count++;
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
    float u;
    float v;
    float t = 0;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        Vertex *v = &mesh->vertices[meshloader->vertex++];
        v->position.x = x;
        v->position.y = y;
        v->position.z = z;
        //v->texture.x = (float)(meshloader->vertex % 3)/3.0;
        //v->texture.y = (float)((meshloader->vertex / 3) % 3)/3.0;
    }
    if (sscanf(buf, "vt %f %f %f", &u, &v, &t) > 1) {
        Vector2f *t = &meshloader->textures[meshloader->texture++];
        t->x = u;
        t->y = v;
    }
    if (sscanf(buf, "vn %f %f %f", &x, &y, &z) > 2) {
        Vector3f *n = &meshloader->normals[meshloader->normal++];
        n->x = x;
        n->y = y;
        n->z = z;
    }

    int index_count = sscanf(buf, "f %s %s %s %s", s[0], s[1], s[2], s[3]);
    int index[4];
    for (int i = 0; i < index_count; i++) {
        FaceData face_data;
        _extract_face_data(s[i], &face_data);
        if (face_data.v > mesh->vertex_count) {
            printf("WARNING: Ignoring out of bounds vertex reference %d out of %d\n  Line: '%s'", face_data.v, mesh->vertex_count, buf);
            continue;
        }
        index[i] = face_data.v;
        int vertex_index = face_data.v - 1;
        Vertex *v = &meshloader->mesh->vertices[vertex_index];

        if (face_data.t > meshloader->texture_count) {
            printf("WARNING: Ignoring out of bounds texture reference %d out of %d\n  Line: '%s'", face_data.t, meshloader->texture_count, buf);
            continue;
        } else {
            Vector2f *t = &meshloader->textures[face_data.t-1];
            if (meshloader->vertex_writes[vertex_index]) {
                meshloader->texture_overwrites++;
            }
            meshloader->vertex_writes[vertex_index] = true;
            v->texture.x = t->x;
            v->texture.y = t->y;
        }
        if (face_data.n > meshloader->normal_count) {
            printf(
                "WARNING: Ignoring out of bounds normal reference %d out "
                "of %d\n  Line: '%s'",
                face_data.n, meshloader->normal_count, buf);
            continue;
        } else {
            Vector3f *n = &meshloader->normals[face_data.n-1];
            v->normal.x = n->x;
            v->normal.y = n->y;
            v->normal.z = n->z;
        }
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
    printf("Number of texture coordinates: %d\n", meshloader.texture_count);
    printf("Number of normals coordinates: %d\n", meshloader.normal_count);
    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));
    mesh->indices = calloc(mesh->index_count, sizeof(int));
    meshloader.textures = calloc(meshloader.texture_count, sizeof(Vector2f));
    meshloader.vertex_writes = calloc(mesh->vertex_count, sizeof(bool));
    meshloader.normals = calloc(meshloader.normal_count, sizeof(Vector3f));
    fseek(fp, 0L, SEEK_SET);    
    while (!feof(fp)) {
        fgets(buf, bufsize, fp);
        _mesh_loader_populate(&meshloader, buf);
        if (meshloader.mesh->vertices == NULL) {
            break;
        }
    }
    fclose(fp);    
    free(meshloader.textures);
    free(meshloader.normals);
    free(meshloader.vertex_writes);
    printf("Texture overwrites: %d\n", meshloader.texture_overwrites);
    return mesh;

}
