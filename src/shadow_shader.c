#include <GL/glew.h>
#include <stdlib.h>

#include "shader_program.h"
#include "shadow_shader.h"
#include "uniform.h"

typedef struct {
    GLuint shadow_program;
    GLint camera;
    GLint world;
    GLint projection;
} ShadowVariables;

struct _ShadowShader {
    Shadowmap *shadowmap;
    ShadowVariables vars;
};

ShadowShader *shadow_shader_create(Shadowmap *shadowmap) {
    ShadowShader *shader = calloc(1, sizeof(ShadowShader));
    shader->shadowmap = shadowmap;
    if (!(shader->vars.shadow_program = shader_program_build("shadow.vs", "shadow.fs")) ||
        !uniform_assign(shader->vars.shadow_program, &shader->vars.world, "gWorld") ||
        !uniform_assign(shader->vars.shadow_program, &shader->vars.camera, "gCamera") ||        
        !uniform_assign(shader->vars.shadow_program, &shader->vars.projection, "gProjection")) {

        shadow_shader_destroy(shader);
        return NULL;
    }

    return shader;
}


void shadow_shader_select(ShadowShader *shader) {
    glUseProgram(shader->vars.shadow_program);
    shadowmap_set_as_render_target(shader->shadowmap);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void shadow_shader_set_projection(ShadowShader *shader, Matrix4f *projection) {
    glProgramUniformMatrix4fv(shader->vars.shadow_program, shader->vars.projection, 1, GL_TRUE, &projection->m[0][0]);
}

void shadow_shader_set_camera(ShadowShader *shader, Matrix4f *camera) {
    glProgramUniformMatrix4fv(shader->vars.shadow_program, shader->vars.camera, 1, GL_TRUE, &camera->m[0][0]);
}

void shadow_shader_set_world(ShadowShader *shader, Matrix4f *world) {
    glProgramUniformMatrix4fv(shader->vars.shadow_program, shader->vars.world, 1, GL_TRUE, &world->m[0][0]);
}

void shadow_shader_destroy(ShadowShader *shader) {
    if (shader == NULL) {
        return;
    }
    free(shader);
}
