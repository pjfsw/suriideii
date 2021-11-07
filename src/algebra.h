#ifndef _ALGEBRA_H
#define _ALGEBRA_H

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

typedef struct {
    float m[4][4];
} Matrix4f;

void vector3f_set(Vector3f *v, float x, float y, float z);

void vector3f_zero(Vector3f *v);

void vector3f_x(Vector3f *v, float x);

void vector3f_y(Vector3f *v, float y);

void vector3f_z(Vector3f *v, float z);


void vector3f_normalize(Vector3f *v);

float vector3f_length(Vector3f *v);

// Returns target
Vector3f *vector3f_copy(Vector3f *src, Vector3f *target);

// Returns target
Vector3f *vector3f_add(Vector3f *src, Vector3f *target);

// Returns target
Vector3f *vector3f_cross(Vector3f *lhs, Vector3f *rhs, Vector3f *target);

// Returns target
Vector3f *vector3f_multiply_scalar(float scalar, Vector3f *src, Vector3f *target);


void matrix4f_set(Matrix4f *m, float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24, float m31, float m32, float m33,
    float m34, float m41, float m42, float m43, float m44);

void matrix4f_rotation(Matrix4f *m, float ax, float ay, float az);

void matrix4f_scale(Matrix4f *m, float sx, float sy, float sz);

void matrix4f_translation(Matrix4f *m, float x, float y, float z);

void matrix4f_perspective(Matrix4f *m, float x_fov, float y_fov, float a, float b);

// rhs = lhs * rhs
void matrix4f_multiply(Matrix4f *lhs, Matrix4f *rhs);

// target = lhs * rhs
void matrix4f_multiply_target(Matrix4f *lhs, Matrix4f *rhs, Matrix4f *target);


#endif