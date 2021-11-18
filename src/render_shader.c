#include <GL/glew.h>

#include "render_shader.h"
#include "shader_program.h"
#include "uniform.h"

typedef struct {
    GLuint render_program;
    GLint camera;
    GLint perspective;
    GLint world;
    GLint sampler;
    GLint camera_pos;
    GLint light_space;
    GLint shadowmap;
    GLint light_projection;
} RenderVariables;

struct _RenderShader {
    RenderVariables vars;
    Lighting *lighting;
    Shadowmap *shadowmap;
};

RenderShader *render_shader_create(Shadowmap *shadowmap) {
    RenderShader *shader = calloc(1, sizeof(RenderShader));
    shader->shadowmap = shadowmap;

    if (!(shader->vars.render_program = shader_program_build("render.vs", "render.fs")) ||
        !uniform_assign(shader->vars.render_program, &shader->vars.camera, "gCamera") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.perspective, "gPerspective") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.world, "gWorld") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.sampler, "gSampler") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.camera_pos, "gCameraPos") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.light_space, "gLightSpace") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.shadowmap, "gShadowMap") ||
        !uniform_assign(shader->vars.render_program, &shader->vars.light_projection, "gLightProjection")) {
        render_shader_destroy(shader);
        return NULL;
    }

    glProgramUniform1i(shader->vars.render_program, shader->vars.sampler, 0);
    if ((shader->lighting = lighting_create(shader->vars.render_program)) == NULL) {
        render_shader_destroy(shader);
        return NULL;
    }

    return shader;
}

void render_shader_set_projection(RenderShader *shader, Matrix4f *projection) {
    glProgramUniformMatrix4fv(shader->vars.render_program, shader->vars.perspective, 1, GL_TRUE, &projection->m[0][0]);
}

void render_shader_set_camera(RenderShader *shader, Matrix4f *camera, Vector3f *camera_pos) {
    glProgramUniformMatrix4fv(shader->vars.render_program, shader->vars.camera, 1, GL_TRUE, &camera->m[0][0]);
    glProgramUniform3f(shader->vars.render_program, shader->vars.camera_pos, camera_pos->x, camera_pos->y, camera_pos->z);
}

void render_shader_set_world(RenderShader *shader, Matrix4f *world) {
    glProgramUniformMatrix4fv(shader->vars.render_program, shader->vars.world, 1, GL_TRUE, &world->m[0][0]);
}

void render_shader_set_light_view(RenderShader *shader, Matrix4f *light_view) {
    glProgramUniformMatrix4fv(shader->vars.render_program, shader->vars.light_space, 1, GL_TRUE, &light_view->m[0][0]);    
}

void render_shader_set_light_projection(RenderShader *shader, Matrix4f *projection) {
    glProgramUniformMatrix4fv(shader->vars.render_program, shader->vars.light_projection, 1, GL_TRUE, &projection->m[0][0]);
}

Lighting *render_shader_get_lighting(RenderShader *shader) {
    return shader->lighting;
}

void render_shader_select(RenderShader *shader) {    
    glUseProgram(shader->vars.render_program);
    shadowmap_bind(shader->shadowmap, GL_TEXTURE1);
    glUniform1i(shader->vars.shadowmap, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}

void render_shader_bind_texture(Texture *texture) {
    texture_bind(texture, GL_TEXTURE0);
}

void render_shader_destroy(RenderShader *shader) {
    if (shader == NULL) {
        return;
    }
    if (shader->lighting != NULL) {
        lighting_destroy(shader->lighting);
    }
    free(shader);
}

