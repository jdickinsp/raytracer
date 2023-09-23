#ifndef MATRIX44_H
#define MATRIX44_H

#include <vectors.h>

// uses row major order
typedef struct {
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
} Matrix44;

void matrix44_debug_print(Matrix44 *m);
void matrix44_set_identity(Matrix44 *m);
Vec3 matrix44_vec3w_mul(Matrix44 *m, Vec3 v);
Vec3 matrix44_vec3_mul(Matrix44 *m, Vec3 v);
Matrix44 matrix44_inverse(Matrix44 *m);

#endif
