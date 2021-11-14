#ifndef _UNIFORM_H
#define _UNIFORM_H

#include <GL/glew.h>
#include <GL/gl.h>

bool uniform_assign(GLuint shader_program, GLint *uniform, char *name);

#endif