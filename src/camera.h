#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <stdbool.h>

#include "algebra.h"

typedef struct {
    Vector3f position;
    Vector3f target;
    Vector3f flat_target;
    Vector3f up;
    Vector3f flat_up;
    Matrix4f m;
    float angle_h;
    float angle_v;
} Camera;

void camera_reset(Camera *camera);

void camera_set(Camera *camera, Vector3f *position, Vector3f *target, Vector3f *up);

void camera_get_move_vector(Camera *camera, bool backward, double move, Vector3f *move_vector);

void camera_get_move_left_vector(Camera *camera, double move, Vector3f *move_vector);

void camera_get_move_right_vector(Camera *camera, double move, Vector3f *move_vector);

void camera_add_movement(Camera *camera, Vector3f *move_vector);

void camera_look(Camera *camera, float dx, float dy);

void camera_transform_rebuild(Camera *camera);

void camera_log(Camera *camera, char *prefix);

#endif

