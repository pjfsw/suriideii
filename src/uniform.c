#include <stdbool.h>
#include <stdio.h>

#include "uniform.h"

bool uniform_assign(GLuint shader_program, GLint *uniform, char *name) {
    glUseProgram(shader_program);

    *uniform = glGetUniformLocation(shader_program, name);
    if (*uniform < 0) {
        fprintf(stderr, "Failed to get %s variable from shader program\n", name);
        return false;
    }
    return true;
}
