#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <GL/gl.h>

typedef struct {
    GLenum target;
    GLuint texture;
    int width;
    int height;
} Texture;

Texture *texture_create(char *file_name);

void texture_destroy(Texture *texture);

void texture_bind(Texture *texture, GLenum texture_unit);

#endif