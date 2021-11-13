#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#include <GL/glew.h>
#include <GL/gl.h>

typedef struct {
    GLuint fbo;
    GLuint shadowmap;
} Shadowmap;

Shadowmap *shadowmap_create(int width, int height);

void shadowmap_destroy(Shadowmap *shadowmap);

void shadowmap_bind(Shadowmap *shadowmap, GLenum texture_unit);

void shadowmap_set_as_render_target(Shadowmap *shadowmap);

#endif