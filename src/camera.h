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

// Move along the target direction
void camera_move_unrestrained(Camera *camera, double delta);

// Move along the ground
void camera_move(Camera *camera, bool backward, double delta_time);

void camera_move_left(Camera *camera, double delta_time);

void camera_move_right(Camera *camera, double delta_time);

// Normalized screen coordinates
void camera_look(Camera *camera, float dx, float dy);

void camera_transform_rebuild(Camera *camera);


#endif

