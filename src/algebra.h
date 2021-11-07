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

void matrix4f_set(Matrix4f *m, float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24, float m31, float m32, float m33,
    float m34, float m41, float m42, float m43, float m44);


void matrix4f_rotation(Matrix4f *m, float ax, float ay, float az);

void matrix4f_scale(Matrix4f *m, float sx, float sy, float sz);

void matrix4f_translation(Matrix4f *m, float x, float y, float z);

void matrix4f_perspective(Matrix4f *m, float x_fov, float y_fov, float a, float b);

void matrix4f_multiply(Matrix4f *lhs, Matrix4f *rhs, Matrix4f *target);

#endif