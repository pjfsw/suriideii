#include "algebra.h"
#include "math.h"
#include "string.h"

void vector3f_set(Vector3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vector3f_zero(Vector3f *v) {
    v->x = 0;
    v->y = 0;
    v->z = 0;
}

void vector3f_x(Vector3f *v, float x) {
    v->x = x;
    v->y = 0;
    v->z = 0;
}

void vector3f_y(Vector3f *v, float y) {
    v->x = 0;
    v->y = y;
    v->z = 0;
}

void vector3f_z(Vector3f *v, float z) {
    v->x = 0;
    v->y = 0;
    v->z = z;
}

float vector3f_length(Vector3f *v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vector3f_normalize(Vector3f *v) {
    float length = vector3f_length(v);

    v->x = v->x / length;
    v->y = v->y / length;
    v->z = v->z / length;
}

Vector3f *vector3f_copy(Vector3f *src, Vector3f *target) {
    target->x = src->x;
    target->y = src->y;
    target->z = src->z;
    return target;
}

Vector3f *vector3f_add(Vector3f *src, Vector3f *target) {
    target->x += src->x;
    target->y += src->y;
    target->z += src->z;
    return target;
}

Vector3f *vector3f_cross(Vector3f *lhs, Vector3f *rhs, Vector3f *target) {
    target->x = lhs->y * rhs->z - lhs->z * rhs->y;
    target->y = lhs->z * rhs->x - lhs->x * rhs->z;
    target->z = lhs->x * rhs->y - lhs->y * rhs->x;
    return target;
}

Vector3f *vector3f_multiply_scalar(float scalar, Vector3f *src, Vector3f *target) {
    target->x = scalar * src->x;
    target->y = scalar * src->y;
    target->z = scalar * src->z;
    return target;
}


void matrix4f_set(Matrix4f *m, float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24, float m31, float m32, float m33,
    float m34, float m41, float m42, float m43, float m44) {
    // Row 1
    m->m[0][0] = m11;
    m->m[0][1] = m12;
    m->m[0][2] = m13;
    m->m[0][3] = m14;
    // Row 2
    m->m[1][0] = m21;
    m->m[1][1] = m22;
    m->m[1][2] = m23;
    m->m[1][3] = m24;
    // Row 3
    m->m[2][0] = m31;
    m->m[2][1] = m32;
    m->m[2][2] = m33;
    m->m[2][3] = m34;
    // Row 4
    m->m[3][0] = m41;
    m->m[3][1] = m42;
    m->m[3][2] = m43;
    m->m[3][3] = m44;
}

void matrix4f_translation(Matrix4f *m, float x, float y, float z) {
    matrix4f_set(m, 1,0,0,x, 0,1,0,y, 0,0,1,z,0,0,0,1);
}

void matrix4f_rotation(Matrix4f *m, float ax, float ay, float az) {
    ax = ax;
    ay = ay;
    double a = az;
    double b = ay;
    double c = ax;
    //matrix4f_set(m, cos(az), -sin(az), 0, 0, sin(az), cos(az),0,0, 0,0,1,0, 0,0,0,1);
    matrix4f_set(m,
        cos(a) * cos(b), cos(a) * sin(b) * sin(c) - sin(a) * cos(c),
        cos(a) * sin(b) * cos(c) + sin(a) * sin(c), 0,

        sin(a) * cos(b), sin(a) * sin(b) * sin(c) + cos(a) * cos(c),
        sin(a) * sin(b) * cos(c) - cos(a) * sin(c), 0,

        -sin(b), cos(b) * sin(c), cos(b) * cos(c), 0, 
        0,0,0,1);
}

void matrix4f_scale(Matrix4f *m, float sx, float sy, float sz) {
    matrix4f_set(m, sx,0,0,0, 0,sy,0,0, 0,0,sz,0, 0,0,0,1);
}

void _matrix4f_multiply(float lhs[4][4], float rhs[4][4], float target[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            target[i][j] = 0;
            for (int n = 0; n < 4; n++) {
                target[i][j] += lhs[i][n] * rhs[n][j];
            }
        }
    }
}

void matrix4f_multiply(Matrix4f *lhs, Matrix4f *rhs) {
    float dst[4][4];
    _matrix4f_multiply(lhs->m, rhs->m, dst);
    memcpy(rhs, dst, sizeof(float) * 16);
}

void matrix4f_multiply_target(Matrix4f *lhs, Matrix4f *rhs, Matrix4f *target) {
    _matrix4f_multiply(lhs->m, rhs->m, target->m);
}

void matrix4f_perspective(Matrix4f *m, float x_fov, float y_fov, float a, float b) {
    float x2 = 0.5 * x_fov;
    float y2 = 0.5 * y_fov;
    matrix4f_set(m, 1/tan(x2), 0, 0, 0, 0, 1/tan(y2), 0, 0, 0,0,a,b,0,0,1,0);
}
