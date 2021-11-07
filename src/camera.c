#include <stdio.h>
#include <GL/freeglut_std.h>
#include "algebra.h"
#include "camera.h"

void camera_reset(Camera *camera) {
    vector3f_zero(&camera->position);
    vector3f_z(&camera->target, 1);
    vector3f_y(&camera->up, 1);
    camera_transform_rebuild(camera);
}

float _camera_get_movement(double delta_time) {
    return 10.0 * delta_time;
}


void camera_move(Camera *camera, bool backward, double delta_time) {
    double move = _camera_get_movement(delta_time);
    if (backward) {
        move = -1.0 * move;
    }
    Vector3f temp;
    vector3f_add(vector3f_multiply_scalar(move, &camera->target, &temp),
        &camera->position);
}

void camera_move_left(Camera *camera, double delta_time) {
    double move = _camera_get_movement(delta_time);
    Vector3f left;
    vector3f_cross(&camera->target, &camera->up, &left);
    vector3f_normalize(&left);
    vector3f_multiply_scalar(move, &left, &left); 
    vector3f_add(&left, &camera->position);
}

void camera_move_right(Camera *camera, double delta_time) {
    double move = _camera_get_movement(delta_time);
    Vector3f right;
    vector3f_cross(&camera->up, &camera->target, &right);
    vector3f_normalize(&right);
    vector3f_multiply_scalar(move, &right, &right); 
    vector3f_add(&right, &camera->position);
}


void camera_transform_rebuild(Camera *camera) {
    Vector3f n;
    vector3f_copy(&camera->target, &n);
    vector3f_normalize(&n);
    Vector3f u;
    vector3f_cross(&camera->up, &n, &u);
    vector3f_normalize(&u);
    Vector3f v;
    vector3f_cross(&n, &u, &v);

    matrix4f_set(&camera->m, 
      u.x, u.y, u.z, -camera->position.x,
      v.x, v.y, v.z, -camera->position.y,
      n.x, n.y, n.z, -camera->position.z,
      0, 0, 0, 1);
}
