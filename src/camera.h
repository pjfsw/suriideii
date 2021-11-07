#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <stdbool.h>

#include "algebra.h"

typedef struct {
    Vector3f position;
    Vector3f target;
    Vector3f up;
    Matrix4f m;
} Camera;

void camera_reset(Camera *camera);

void camera_move(Camera *camera, bool backward, double delta_time);

void camera_move_left(Camera *camera, double delta_time);

void camera_move_right(Camera *camera, double delta_time);

void camera_transform_rebuild(Camera *camera);

#endif

