#include <math.h>
#include <stdio.h>
#include <string.h>

#include "algebra.h"

void vector2f_set(Vector2f *v, float x, float y) {
    v->x = x;
    v->y = y;
}

void vector2f_copy(Vector2f *src, Vector2f *target) {
    target->x = src->x;
    target->y = src->y;
}

char *vector2f_to_string(Vector2f *v, char *output) {
    sprintf(output, "(%f,%f)", v->x, v->y);
    return output;
}

char *vector3f_to_string(Vector3f *v, char *output) {
    sprintf(output, "(%f,%f,%f)", v->x, v->y, v->z);
    return output;
}

void vector3f_set(Vector3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vector3f_set_and_normalize(Vector3f *v, float x, float y, float z) {
    vector3f_set(v, x, y, z);
    vector3f_normalize(v);
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

void vector3f_rotate(Vector3f *to_rotate, float angle, Vector3f *axis) {
    Quaternion rotation_q;
    quaternion_from_vector(&rotation_q, angle, axis);
    Quaternion conjugate_q;
    quaternion_conjugate(&rotation_q, &conjugate_q);
    Quaternion w;
    quaternion_multiply_vector(&rotation_q, to_rotate, &w);
    Quaternion w2;
    quaternion_multiply_quaternion(&w, &conjugate_q, &w2);
    to_rotate->x = w2.x;
    to_rotate->y = w2.y;
    to_rotate->z = w2.z;
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

bool _algebra_float_equals(float a, float b) {
    float diff = a-b;
    return fabs(diff) < 0.0001;
}

bool vector2f_equals(Vector2f *v1,  Vector2f *v2) {
    return _algebra_float_equals(v1->x, v2->x) &&
           _algebra_float_equals(v1->y, v2->y);
}

bool vector3f_equals(Vector3f *v1,  Vector3f *v2) {
    return _algebra_float_equals(v1->x, v2->x) &&
           _algebra_float_equals(v1->y, v2->y) &&
           _algebra_float_equals(v1->z, v2->z);
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

void matrix4f_perspective(Matrix4f *m, float fov, float ar, float a, float b) {
    float tan_half_fov = 1/tan(0.5 * fov); 
    matrix4f_set(m, 1/(tan_half_fov), 0, 0, 0, 0, 1/(tan_half_fov * ar), 0, 0, 0,0,a,b,0,0,1,0);
}

void matrix4f_multiply_vector(Matrix4f *lhs, Vector3f *rhs) {
    float dst[4];
    for (int i = 0; i < 4; i++) {
        dst[i] = lhs->m[i][0] * rhs->x + lhs->m[i][1] * rhs->y + lhs->m[i][2] * rhs->z + lhs->m[i][3];
    }
    rhs->x = dst[0];
    rhs->y = dst[1];
    rhs->z = dst[2];
}

void quaternion_set(Quaternion *quaternion, float x, float y, float z, float w) {
    quaternion->x = x;
    quaternion->y = y;
    quaternion->z = z;
    quaternion->w = w;
}

void quaternion_from_vector(Quaternion *quaternion, float angle, Vector3f *v) {
    float half_angle = angle / 2.0f;
    float sin_half_angle = sinf(half_angle);
    float cos_half_angle = cosf(half_angle);

    quaternion->x = v->x * sin_half_angle;
    quaternion->y = v->y * sin_half_angle;
    quaternion->z = v->z * sin_half_angle;
    quaternion->w = cos_half_angle;
}

void quaternion_normalize(Quaternion *quaternion) {
    float length =
        sqrtf(quaternion->x * quaternion->x + quaternion->y * quaternion->y +
              quaternion->z + quaternion->z + quaternion->w + quaternion->w);

    quaternion->x /= length;
    quaternion->y /= length;
    quaternion->z /= length;
    quaternion->w /= length;
}

Quaternion *quaternion_conjugate(Quaternion *src, Quaternion *target) {
    target->x = -src->x;
    target->y = -src->y;
    target->z = -src->z;
    target->w = src->w;    
    return target;
}

// Multiplys Q (lhs) * V (rhs) and stores in target, returns target
Quaternion *quaternion_multiply_vector(Quaternion *q, Vector3f *v, Quaternion *target) {
    target->w = -(q->x * v->x) - (q->y * v->y) - (q->z * v->z);
    target->x = (q->w * v->x) + (q->y * v->z) - (q->z * v->y);
    target->y = (q->w * v->y) + (q->z * v->x) - (q->x * v->z);
    target->z = (q->w * v->z) + (q->x * v->y) - (q->y * v->x);

    return target;
}


// Multiplys Q1 (lhs) * Q2 (rhs) and stores in target, returns target
Quaternion *quaternion_multiply_quaternion(Quaternion *lhs, Quaternion *rhs, Quaternion *target) {
    target->w = (lhs->w * rhs->w) - (lhs->x * rhs->x) - (lhs->y * rhs->y) - (lhs->z * rhs->z);
    target->x = (lhs->x * rhs->w) + (lhs->w * rhs->x) + (lhs->y * rhs->z) - (lhs->z * rhs->y);
    target->y = (lhs->y * rhs->w) + (lhs->w * rhs->y) + (lhs->z * rhs->x) - (lhs->x * rhs->z);
    target->z = (lhs->z * rhs->w) + (lhs->w * rhs->z) + (lhs->x * rhs->y) - (lhs->y * rhs->x);
    
    return target;    
}
