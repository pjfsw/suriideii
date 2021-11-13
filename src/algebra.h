#ifndef _ALGEBRA_H
#define _ALGEBRA_H

#include <stdbool.h>

typedef struct {
    float x;
    float y;
} Vector2f;

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

typedef struct {
    float m[4][4];
} Matrix4f;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Quaternion;

void vector2f_set(Vector2f *v, float x, float y);

void vector2f_copy(Vector2f *src, Vector2f *target);

bool vector2f_equals(Vector2f *v1,  Vector2f *v2);

char *vector2f_to_string(Vector2f *v, char *output);

void vector3f_set(Vector3f *v, float x, float y, float z);

void vector3f_set_and_normalize(Vector3f *v, float x, float y, float z);

void vector3f_zero(Vector3f *v);

void vector3f_x(Vector3f *v, float x);

void vector3f_y(Vector3f *v, float y);

void vector3f_z(Vector3f *v, float z);

void vector3f_normalize(Vector3f *v);

float vector3f_length(Vector3f *v);

void vector3f_rotate(Vector3f *to_rotate, float angle, Vector3f *axis);

// Returns target
Vector3f *vector3f_copy(Vector3f *src, Vector3f *target);

// Returns target
Vector3f *vector3f_add(Vector3f *src, Vector3f *target);

// Returns target
Vector3f *vector3f_cross(Vector3f *lhs, Vector3f *rhs, Vector3f *target);

// Returns target
Vector3f *vector3f_multiply_scalar(float scalar, Vector3f *src, Vector3f *target);

char *vector3f_to_string(Vector3f *v, char *output);

bool vector3f_equals(Vector3f *v1,  Vector3f *v2);

void matrix4f_set(Matrix4f *m, float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24, float m31, float m32, float m33,
    float m34, float m41, float m42, float m43, float m44);

void matrix4f_rotation(Matrix4f *m, float ax, float ay, float az);

void matrix4f_scale(Matrix4f *m, float sx, float sy, float sz);

void matrix4f_translation(Matrix4f *m, float x, float y, float z);

void matrix4f_perspective(Matrix4f *m, float x_fov, float y_fov, float a, float b);

void matrix4f_multiply_vector(Matrix4f *lhs, Vector3f *rhs);

// rhs = lhs * rhs
void matrix4f_multiply(Matrix4f *lhs, Matrix4f *rhs);

// target = lhs * rhs
void matrix4f_multiply_target(Matrix4f *lhs, Matrix4f *rhs, Matrix4f *target);

void quaternion_set(Quaternion *quaternion, float x, float y, float z, float w);

// 
void quaternion_from_vector(Quaternion *quaternion, float angle, Vector3f *v);

// Normalizes the quaternion
void quaternion_normalize(Quaternion *quaternion);

// Creates the conjugate of src in target and returns target
Quaternion *quaternion_conjugate(Quaternion *src, Quaternion *target);

// Multiplys Q * V and stores in target, returns target
Quaternion *quaternion_multiply_vector(Quaternion *q, Vector3f *v, Quaternion *target);

// Multiplys Q1 (lhs) * Q2 (rhs) and stores in target, returns target
Quaternion *quaternion_multiply_quaternion(Quaternion *lhs, Quaternion *rhs, Quaternion *target);

#endif