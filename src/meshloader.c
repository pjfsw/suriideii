#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "meshloader.h"

typedef struct {
    int vertex;
    int normal;
    int texture;
} MeshIndex;

typedef struct {
    Mesh *mesh;
    // vertices
    int vertex;
    int vertex_count;
    Vector3f *vertices;    
    // textures
    int texture;
    int texture_count;
    Vector2f *textures;
    // normals    
    int normal;
    int normal_count;
    Vector3f *normals;
    // indices
    int index;
    int index_count;
    MeshIndex *indices;
} Meshloader;

typedef struct {
    int v;
    int t;
    int n;
} FaceData;

void _extract_face_data(char *s, FaceData *face) {
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
}

void _mesh_loader_count(Meshloader *meshloader, char *buf) {
    char s[4][100];

    float x;
    float y;
    float z;
    float u;
    float v; 
    float t;
    float w = 1;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        meshloader->vertex_count++;
    }
    if (sscanf(buf, "vt %f %f %f", &u,&v,&t) > 1) {
        meshloader->texture_count++;
    }
    if (sscanf(buf, "vn %f %f %f", &x, &y, &z) > 2) {
        meshloader->normal_count++;
    }
    int index_count = sscanf(buf, "f %s %s %s %s", s[0], s[1], s[2], s[3]);
    if (index_count == 3) {
        meshloader->index_count += 3;
    } else if (index_count == 4) {
        meshloader->index_count += 6;
    }
}

void _mesh_loader_populate(Meshloader *meshloader, char *buf) {
    char s[4][100];

    float x;
    float y;
    float z;
    float w = 1;
    float u;
    float v;
    float t = 0;

    if (sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w) > 2) {
        Vector3f *v = &meshloader->vertices[meshloader->vertex++];
        vector3f_set(v, x,y,z);
    }
    if (sscanf(buf, "vn %f %f %f", &x, &y, &z) > 2) {
        Vector3f *n = &meshloader->normals[meshloader->normal++];
        n->x = x;
        n->y = y;
        n->z = z;
    }
    if (sscanf(buf, "vt %f %f %f", &u, &v, &t) > 1) {
        Vector2f *tex = &meshloader->textures[meshloader->texture++];
        tex->x = u;
        tex->y = v;
    }

    int index_count = sscanf(buf, "f %s %s %s %s", s[0], s[1], s[2], s[3]);
    MeshIndex index[4];
    for (int i = 0; i < index_count; i++) {
        FaceData face_data;
        _extract_face_data(s[i], &face_data);
        index[i].vertex = face_data.v-1;
        index[i].normal = face_data.n-1;
        index[i].texture = face_data.t-1;
    }
    if (index_count >= 3) {
        memcpy(&meshloader->indices[meshloader->index++], &index[0], sizeof(MeshIndex));
        memcpy(&meshloader->indices[meshloader->index++], &index[1], sizeof(MeshIndex));
        memcpy(&meshloader->indices[meshloader->index++], &index[2], sizeof(MeshIndex));
    } 
    if (index_count == 4) {
        memcpy(&meshloader->indices[meshloader->index++], &index[0], sizeof(MeshIndex));
        memcpy(&meshloader->indices[meshloader->index++], &index[2], sizeof(MeshIndex));
        memcpy(&meshloader->indices[meshloader->index++], &index[3], sizeof(MeshIndex));
    }
}

bool _mesh_index_equals(MeshIndex *one, MeshIndex *other) {
    return one->normal == other->normal && one->texture == other->texture &&
           one->vertex == other->vertex;
}

void _mesh_create_vertex(Meshloader *meshloader, Vertex *v, MeshIndex *mi, int i) {
    if (mi->vertex < 0 || mi->vertex >= meshloader->vertex_count) {
        printf("Invalid vertex reference %d=>%d detected, vertex count is %d\n", i, mi->vertex+1, meshloader->vertex_count);
        vector3f_zero(&v->position);
    } else {
        vector3f_copy(&meshloader->vertices[mi->vertex], &v->position);
    }
    if (mi->normal < 0 || mi->normal >= meshloader->normal_count) {
        printf("Invalid normal reference %d=>%d detected, normal count is %d\n", i, mi->normal+1, meshloader->normal_count);
        vector3f_set(&v->normal, 0, 1, 0);
    } else {
        vector3f_copy(&meshloader->normals[mi->normal], &v->normal);
    }
    if (mi->texture < 0 || mi->texture >= meshloader->texture_count) {
        printf("Invalid texture reference %d=>%d detected, texture count is %d\n", i, mi->texture+1, meshloader->texture_count);
        vector2f_set(&v->texture, 0, 0);
    } else {
        vector2f_copy(&meshloader->textures[mi->texture], &v->texture);
    }
}

void _mesh_loader_generate(Meshloader *meshloader) {
    Mesh *mesh = meshloader->mesh;

    mesh->vertex_count = meshloader->vertex_count;
    bool use_texture_index = false;
    if (meshloader->texture_count > mesh->vertex_count) {        
        mesh->vertex_count = meshloader->texture_count;
        use_texture_index = true;
    }

    mesh->vertices = calloc(mesh->vertex_count, sizeof(Vertex));
    mesh->index_count = meshloader->index_count;
    mesh->indices = calloc(mesh->index_count, sizeof(int));

    bool *used = calloc(mesh->vertex_count, sizeof(bool));

    char out[6][50];

    int copies = 0;
    for (int i = 0; i < mesh->index_count; i++) {
        copies = i+1;
        MeshIndex *mi = &meshloader->indices[i];
        int index = use_texture_index ? mi->texture : mi->vertex;
        if (index < 0 || index >= mesh->vertex_count) {
            printf("Invalid vertex\n");
        } else {
            Vertex v;
            _mesh_create_vertex(meshloader, &v, mi, i);
            Vertex *other = &mesh->vertices[index];
            if (used[index]) {
                if (!vector3f_equals(&v.position, &other->position) ||
                    !vector3f_equals(&v.normal, &other->normal) ||
                    !vector2f_equals(&v.texture, &other->texture)) {
                    printf(
                        "Warning reusing vertex with different properties "
                        "(V1=%s,N1=%s,T1=%s) <-> (V2=%s,N2=%s,T2=%s)\n",
                        vector3f_to_string(&v.position, out[0]),
                        vector3f_to_string(&v.normal, out[1]),
                        vector2f_to_string(&v.texture, out[2]),
                        vector3f_to_string(&other->position, out[3]),
                        vector3f_to_string(&other->normal, out[4]),
                        vector2f_to_string(&other->texture, out[5]));
                }
            }
            memcpy(other, &v, sizeof(Vertex));
            used[index] = true;
            mesh->indices[i] = index;
        }
    }
    printf("Copied %d indices\n", copies);
    free(used);
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
    printf("Detected %d vertices and %d indices\n", meshloader.vertex_count, meshloader.index_count);
    printf("Number of texture coordinates: %d\n", meshloader.texture_count);
    printf("Number of normals coordinates: %d\n", meshloader.normal_count);
    meshloader.textures = calloc(meshloader.texture_count, sizeof(Vector2f));
    meshloader.normals = calloc(meshloader.normal_count, sizeof(Vector3f));
    meshloader.vertices = calloc(meshloader.vertex_count, sizeof(Vector3f));
    meshloader.indices = calloc(meshloader.index_count, sizeof(MeshIndex));
    fseek(fp, 0L, SEEK_SET);    
    int n = 0;
    while (!feof(fp)) {
        fgets(buf, bufsize, fp);
        _mesh_loader_populate(&meshloader, buf);
        n++;
    }
    fclose(fp);    
    fflush(stdout);
    _mesh_loader_generate(&meshloader);

    printf("Mesh loaded successfully!\n");
    FILE *logfile = fopen("mesh.log", "w");
    char out[3][50];
    if (logfile != NULL) {
        for (int i = 0; i < mesh->index_count; i++) {
            fprintf(logfile, "[V=%s N=%s T=%s] ",
                vector3f_to_string(
                    &mesh->vertices[mesh->indices[i]].position, out[0]),
                vector3f_to_string(
                    &mesh->vertices[mesh->indices[i]].normal, out[1]),
                vector2f_to_string(
                    &mesh->vertices[mesh->indices[i]].texture, out[2]));
            if (i % 3 == 2) {
                fprintf(logfile, "\n");
            }
        }
        fprintf(logfile, "\n");
        fclose(logfile);
    } else {
        fprintf(stderr, "could not write logfile\n");
    }
    logfile = fopen("original.log", "w");
    if (logfile != NULL) {
        for (int i = 0; i < meshloader.index_count; i++) {
            MeshIndex *mi = &meshloader.indices[i];

            fprintf(logfile, "[V=%s N=%s T=%s] ",
                vector3f_to_string(&meshloader.vertices[mi->vertex], out[0]),
                vector3f_to_string(&meshloader.normals[mi->normal], out[1]),
                vector2f_to_string(&meshloader.textures[mi->texture], out[2]));
                if (i % 3 == 2) {
                    fprintf(logfile, "\n");
                }
        }
        fprintf(logfile, "\n");
        fclose(logfile);
    }

    free(meshloader.textures);
    free(meshloader.normals);
    free(meshloader.vertices);
    free(meshloader.indices);
    return mesh;

}
