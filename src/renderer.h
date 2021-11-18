#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "object.h"

typedef struct _Renderer Renderer;

Renderer *renderer_create(double fov);

void renderer_set_camera(Renderer *renderer, Matrix4f *camera, Vector3f *camera_pos);

void renderer_draw(Renderer *renderer, Object **objects, int object_count);

void renderer_destroy(Renderer *renderer);


#endif