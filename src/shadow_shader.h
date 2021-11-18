#ifndef _SHADOW_SHADER_H
#define _SHADOW_SHADER_H

#include "algebra.h"
#include "shadowmap.h"

typedef struct _ShadowShader ShadowShader;

ShadowShader *shadow_shader_create(Shadowmap *shadowmap);

void shadow_shader_select(ShadowShader *shader);

void shadow_shader_set_projection(ShadowShader *shader, Matrix4f *projection);

void shadow_shader_set_camera(ShadowShader *shader, Matrix4f *camera);

void shadow_shader_set_world(ShadowShader *shader, Matrix4f *world);

void shadow_shader_destroy(ShadowShader *shader);

#endif