#include <GL/glew.h>
#include <stdlib.h>

#include "hud_shader.h"

struct _HudShader {
    GLint sampler;
    GLint world;
    GLint ar;
};

HudShader *hud_shader_create() {
    HudShader *hud_shader = calloc(1, sizeof(HudShader));
    return hud_shader;
}

void hud_shader_destroy(HudShader *shader) {
    if (shader == NULL) {
        return;
    }
    free(shader);
}

/*
void render_hud(Object *object) {
    MeshGL *gl = &object->mesh->gl;

    glUniform1f(gui.hud_vars.ar, 1);
    glUniformMatrix4fv(gui.hud_vars.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    

    shadowmap_bind(gui.shadowmap, GL_TEXTURE0);
    glUniform1i(gui.hud_vars.sampler, 0);
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    long size = sizeof(Vector3f);
    
    // Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);
    size += sizeof(Vector2f);

    glDrawElements(GL_TRIANGLES, object->mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);

}


void render_debug() {
    glUseProgram(gui.hud_program);   
    glDisable(GL_DEPTH_TEST);
    render_hud(app.hud_object);
    glEnable(GL_DEPTH_TEST);
}
*/
