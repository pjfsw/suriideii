#include "algebra.h"
#include "math.h"
#include "string.h"

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

void matrix4f_multiply(Matrix4f *lhs, Matrix4f *rhs, Matrix4f *target) {
    float dst[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dst[i][j] = lhs->m[i][0] * rhs->m[0][j] +
                        lhs->m[i][1] * rhs->m[1][j] +
                        lhs->m[i][2] * rhs->m[2][j] +
                        lhs->m[i][3] * rhs->m[3][j];
        }
    }
    memcpy(target->m, dst, sizeof(float) * 16);
}

void matrix4f_perspective(Matrix4f *m, float x_fov, float y_fov, float a, float b) {
    float x2 = 0.5 * x_fov;
    float y2 = 0.5 * y_fov;
    matrix4f_set(m, 1/tan(x2), 0, 0, 0, 0, 1/tan(y2), 0, 0, 0,0,a,b,0,0,1,0);
}
