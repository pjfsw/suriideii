#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>


GLuint shader_program_build(char *vs_name, char *fs_name);

#endif