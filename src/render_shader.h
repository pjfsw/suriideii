#ifndef _RENDER_SHADER_H
#define _RENDER_SHADER_H

#include "algebra.h"
#include "lighting.h"
#include "shadowmap.h"
#include "texture.h"

typedef struct _RenderShader RenderShader;

RenderShader *render_shader_create(Shadowmap *shadowmap);

void render_shader_select(RenderShader *shader);

void render_shader_bind_texture(Texture *texture);

void render_shader_set_projection(RenderShader *shader, Matrix4f *projection);

void render_shader_set_camera(RenderShader *shader, Matrix4f *camera, Vector3f *camera_pos);

void render_shader_set_world(RenderShader *shader, Matrix4f *world);

void render_shader_set_light_view(RenderShader *shader, Matrix4f *light_view);

void render_shader_set_light_projection(RenderShader *shader, Matrix4f *projection);

Lighting *render_shader_get_lighting(RenderShader *shader);

void render_shader_destroy(RenderShader *shader);


#endif