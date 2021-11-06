#include "algebra.h"

void vector3f_set(Vector3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
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