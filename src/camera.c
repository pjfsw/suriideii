#include <math.h>
#include <stdio.h>
#include "algebra.h"
#include "camera.h"

void _camera_set_angle(Camera *camera) {
    Vector3f h_target;
    vector3f_set(&h_target, camera->target.x, 0, camera->target.z);
    vector3f_normalize(&h_target);
    
    float angle = asin(fabsf(h_target.z));
    if (h_target.z >= 0) {
        if (h_target.x >= 0) {
            camera->angle_h = 2.0f * M_PI - angle;
        } else {
            camera->angle_h = M_PI + angle;
        }
    } else {
        if (h_target.x >= 0) {
            camera->angle_h = angle;
        } else {
            camera->angle_h = M_PI - angle;
        }
    } 

    camera->angle_v = -asin(camera->target.y);
}

void camera_set(Camera *camera, Vector3f *position, Vector3f *target, Vector3f *up) {
    vector3f_copy(position, &camera->position);
    vector3f_copy(target, &camera->target);
    vector3f_copy(target, &camera->flat_target);
    camera->flat_target.y = 0;
    vector3f_copy(up, &camera->up);
    vector3f_y(&camera->flat_up, 1);
    _camera_set_angle(camera);
    camera_transform_rebuild(camera);
}

void camera_reset(Camera *camera) {
    vector3f_zero(&camera->position);
    vector3f_z(&camera->target, 1);
    vector3f_z(&camera->flat_target, 1);
    vector3f_y(&camera->up, 1);
    vector3f_y(&camera->flat_up, 1);
    _camera_set_angle(camera);
    camera_transform_rebuild(camera);
}

float _camera_get_movement(double delta_time) {
    return 10.0 * delta_time;
}

void camera_move_unrestrained(Camera *camera, double delta) {
    Vector3f temp;
    vector3f_add(vector3f_multiply_scalar(delta, &camera->target, &temp),
        &camera->position);
}

void camera_move(Camera *camera, bool backward, double delta_time) {
    double move = _camera_get_movement(delta_time);
    if (backward) {
        move = -1.0 * move;
    }
    Vector3f temp;
    vector3f_add(vector3f_multiply_scalar(move, &camera->flat_target, &temp),
        &camera->position);
}

void camera_move_left(Camera *camera, double delta_time) {
    double move = _camera_get_movement(delta_time);
    Vector3f left;
    vector3f_cross(&camera->flat_target, &camera->flat_up, &left);
    vector3f_normalize(&left);
    vector3f_multiply_scalar(move, &left, &left); 
    vector3f_add(&left, &camera->position);
}

void camera_move_right(Camera *camera, double delta_time) {
    double move = _camera_get_movement(delta_time);
    Vector3f right;
    vector3f_cross(&camera->flat_up, &camera->target, &right);
    vector3f_normalize(&right);
    vector3f_multiply_scalar(move, &right, &right); 
    vector3f_add(&right, &camera->position);
}

// Normalized screen coordinates
void camera_look(Camera *camera, float dx, float dy) {
    camera->angle_h += dx;
    camera->angle_v += dy;
    float max_vertical = M_PI/2.0f;

    if (camera->angle_v > max_vertical) {
        camera->angle_v = max_vertical;
    } else if (camera->angle_v < -max_vertical) {
        camera->angle_v = -max_vertical;
    }

    Vector3f y_axis;
    vector3f_y(&y_axis, 1.0f);

    Vector3f view;
    // Rotate horizontal angle around the Y-axis
    vector3f_x(&view, 1.0f);
    vector3f_rotate(&view, camera->angle_h, &y_axis);
    vector3f_normalize(&view);

    // Rotate vertical angle around the X-axis
    Vector3f u;
    vector3f_cross(&y_axis, &view, &u);
    vector3f_normalize(&u);
    vector3f_rotate(&view, camera->angle_v, &u);

    vector3f_copy(&view, &camera->target);
    vector3f_copy(&view, &camera->flat_target);
    vector3f_set_and_normalize(&camera->flat_target, camera->flat_target.x, 0, camera->flat_target.z);
    vector3f_cross(&camera->target, &u, &camera->up);
    vector3f_normalize(&camera->up);
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

    Matrix4f rotation; 
    matrix4f_set(&rotation, 
       u.x, u.y, u.z, 0,
       v.x, v.y, v.z, 0,
       n.x, n.y, n.z, 0,
       0,   0,   0,   1
    );

    Matrix4f translation;
    matrix4f_translation(&translation, -camera->position.x, -camera->position.y, -camera->position.z);
    matrix4f_multiply_target(&rotation, &translation, &camera->m);
}

void camera_log(Camera *camera, char *prefix) {
    printf("%s: angle h=%f v=%f\n", prefix, camera->angle_h, camera->angle_v);
    printf("%s: position (%f,%f,%f)\n", prefix, camera->position.x, camera->position.y, camera->position.z);
    printf("%s: target   (%f,%f,%f)\n", prefix, camera->target.x, camera->target.y, camera->target.z);
    printf("%s: up       (%f,%f,%f)\n", prefix, camera->up.x, camera->up.y, camera->up.z);
    for (int i = 0; i < 4; i++) {
        printf("%s: m[%d]     (%f,%f,%f,%f)\n", prefix, i, camera->m.m[i][0],
            camera->m.m[i][1], camera->m.m[i][2], camera->m.m[i][3]);
    }
}
