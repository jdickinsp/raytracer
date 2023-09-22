#include <matrix44.h>

Matrix44 *matrix44_create() {
    Matrix44 *m = malloc(sizeof(Matrix44));
    matrix44_set_identity(m);
    return m;
}

void matrix44_debug_print(Matrix44 *m) {
    printf("[[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]]\n",
        m->m11, m->m12, m->m13, m->m14,
        m->m21, m->m22, m->m23, m->m24,
        m->m31, m->m32, m->m33, m->m34,
        m->m41, m->m42, m->m43, m->m44
    );
}

void matrix44_set_identity(Matrix44 *m) {
    m->m11 = 1;
    m->m22 = 1;
    m->m33 = 1;
    m->m44 = 1;
}

Vec3 matrix44_vec3w_mul(Matrix44 *m, Vec3 v) {
    Vec3 r;
    // r.x = m->m11 * v.x + m->m12 * v.y + m->m13 * v.z + m->m14 * 1;
    // r.y = m->m21 * v.x + m->m22 * v.y + m->m23 * v.z + m->m24 * 1;
    // r.z = m->m31 * v.x + m->m32 * v.y + m->m33 * v.z + m->m34 * 1;
    // float w = m->m41 * v.x + m->m42 * v.y + m->m43 * v.z + m->m44 * 1;

    r.x = m->m11 * v.x + m->m21 * v.y + m->m31 * v.z + m->m41 * 1;
    r.y = m->m12 * v.x + m->m22 * v.y + m->m32 * v.z + m->m42 * 1;
    r.z = m->m13 * v.x + m->m23 * v.y + m->m33 * v.z + m->m43 * 1;
    float w = m->m14 * v.x + m->m24 * v.y + m->m34 * v.z + m->m44 * 1;

    vec3_debug_print(r);

    printf("w: %f\n", w);
    r.x /= w;
    r.y /= w;
    r.z /= w;
    return r;
}

Vec3 matrix44_vec3_mul(Matrix44 *m, Vec3 v) {
    Vec3 r;
    // r.x = m->m11 * v.x + m->m12 * v.y + m->m13 * v.z;
    // r.y = m->m21 * v.x + m->m22 * v.y + m->m23 * v.z;
    // r.z = m->m31 * v.x + m->m32 * v.y + m->m33 * v.z;
    r.x = m->m11 * v.x + m->m21 * v.y + m->m31 * v.z;
    r.y = m->m12 * v.x + m->m22 * v.y + m->m32 * v.z;
    r.z = m->m13 * v.x + m->m23 * v.y + m->m33 * v.z;
    return r;
}

Matrix44 matrix44_inverse(Matrix44 *m) {
    // find inverse by multipling the adjugate matrix by reciprocal of the determinant.
    Matrix44 im;
    float without_row_1 = (m->m22 * m->m33 * m->m44) + (m->m23 * m->m34 * m->m42) + (m->m24 * m->m32 * m->m43)
        - (m->m24 * m->m33 * m->m42) - (m->m23 * m->m32 * m->m44) - (m->m22 * m->m34 * m->m43);
    float without_row_2 = (m->m12 * m->m33 * m->m44) + (m->m13 * m->m34 * m->m42) + (m->m14 * m->m32 * m->m43)
        - (m->m14 * m->m33 * m->m42) - (m->m13 * m->m32 * m->m44) - (m->m12 * m->m34 * m->m43);
    float without_row_3 = (m->m12 * m->m23 * m->m44) + (m->m13 * m->m24 * m->m42) + (m->m14 * m->m22 * m->m43)
        - (m->m14 * m->m23 * m->m42) - (m->m13 * m->m22 * m->m44) - (m->m12 * m->m24 * m->m43);
    float without_row_4 = (m->m12 * m->m23 * m->m34) + (m->m13 * m->m24 * m->m32) + (m->m14 * m->m22 * m->m33)
        - (m->m14 * m->m23 * m->m32) - (m->m13 * m->m22 * m->m34) - (m->m12 * m->m24 * m->m33);
    float determinant = m->m11 * without_row_1 - m->m21 * without_row_2 + m->m31 * without_row_3 - m->m41 * without_row_4;
    float inv_det = 1 / determinant;

    float A11 = (m->m22 * m->m33 * m->m44) + (m->m23 * m->m34 * m->m42) + (m->m24 * m->m32 * m->m43)
        - (m->m24 * m->m33 * m->m42) - (m->m23 * m->m32 * m->m44) - (m->m22 * m->m34 * m->m43);
    float A12 = -(m->m12 * m->m33 * m->m44) - (m->m13 * m->m34 * m->m42) - (m->m14 * m->m32 * m->m43)
        + (m->m14 * m->m33 * m->m42) + (m->m13 * m->m32 * m->m44) + (m->m12 * m->m34 * m->m43);
    float A13 = (m->m12 * m->m23 * m->m44) + (m->m13 * m->m24 * m->m42) + (m->m14 * m->m22 * m->m43)
        - (m->m14 * m->m23 * m->m42) - (m->m13 * m->m22 * m->m44) - (m->m12 * m->m24 * m->m43);
    float A14 = -(m->m12 * m->m23 * m->m34) - (m->m13 * m->m24 * m->m32) - (m->m14 * m->m22 * m->m33)
        + (m->m14 * m->m23 * m->m32) + (m->m13 * m->m22 * m->m34) + (m->m12 * m->m24 * m->m33);
    
    float A21 = -(m->m21 * m->m33 * m->m44) - (m->m23 * m->m34 * m->m41) - (m->m24 * m->m31 * m->m43)
        + (m->m24 * m->m33 * m->m41) + (m->m23 * m->m31 * m->m44) + (m->m21 * m->m34 * m->m43);
    float A22 = (m->m11 * m->m33 * m->m44) + (m->m13 * m->m34 * m->m41) + (m->m14 * m->m31 * m->m43)
        - (m->m14 * m->m33 * m->m41) - (m->m13 * m->m31 * m->m44) - (m->m11 * m->m34 * m->m43);
    float A23 = -(m->m11 * m->m23 * m->m44) - (m->m13 * m->m24 * m->m41) - (m->m14 * m->m21 * m->m43)
        + (m->m14 * m->m23 * m->m41) + (m->m13 * m->m21 * m->m44) + (m->m11 * m->m24 * m->m43);
    float A24 = (m->m11 * m->m23 * m->m34) + (m->m13 * m->m24 * m->m31) + (m->m14 * m->m21 * m->m33)
        - (m->m14 * m->m23 * m->m31) - (m->m13 * m->m21 * m->m34) - (m->m11 * m->m24 * m->m33);

    float A31 = (m->m21 * m->m32 * m->m44) + (m->m22 * m->m34 * m->m41) + (m->m24 * m->m31 * m->m42)
        - (m->m24 * m->m32 * m->m41) - (m->m22 * m->m31 * m->m44) - (m->m21 * m->m34 * m->m42);
    float A32 = -(m->m11 * m->m32 * m->m44) - (m->m12 * m->m34 * m->m41) - (m->m14 * m->m31 * m->m42)
        + (m->m14 * m->m32 * m->m41) + (m->m12 * m->m31 * m->m44) + (m->m11 * m->m34 * m->m42);
    float A33 = (m->m11 * m->m22 * m->m44) + (m->m12 * m->m24 * m->m41) + (m->m14 * m->m21 * m->m42)
        - (m->m14 * m->m22 * m->m41) - (m->m12 * m->m21 * m->m44) - (m->m11 * m->m24 * m->m42);
    float A34 = -(m->m11 * m->m22 * m->m34) - (m->m12 * m->m24 * m->m31) - (m->m14 * m->m21 * m->m32)
        + (m->m14 * m->m22 * m->m31) + (m->m12 * m->m21 * m->m34) + (m->m11 * m->m24 * m->m32);
    
    float A41 = -(m->m21 * m->m32 * m->m43) - (m->m22 * m->m33 * m->m41) - (m->m23 * m->m31 * m->m42)
        + (m->m23 * m->m32 * m->m41) + (m->m22 * m->m31 * m->m43) + (m->m21 * m->m33 * m->m42);
    float A42 = (m->m11 * m->m32 * m->m43) + (m->m12 * m->m33 * m->m41) + (m->m13 * m->m31 * m->m42)
        - (m->m13 * m->m32 * m->m41) - (m->m12 * m->m31 * m->m43) - (m->m11 * m->m33 * m->m42);
    float A43 = -(m->m11 * m->m22 * m->m43) - (m->m12 * m->m23 * m->m41) - (m->m13 * m->m21 * m->m42)
        + (m->m13 * m->m22 * m->m41) + (m->m12 * m->m21 * m->m43) + (m->m11 * m->m23 * m->m42);
    float A44 = (m->m11 * m->m22 * m->m33) + (m->m12 * m->m23 * m->m31) + (m->m13 * m->m21 * m->m32)
        - (m->m13 * m->m22 * m->m31) - (m->m12 * m->m21 * m->m33) - (m->m11 * m->m23 * m->m32);
    
    im.m11 = A11 * inv_det, im.m12 = A12 * inv_det, im.m13 = A13 * inv_det, im.m14 = A14 * inv_det;
    im.m21 = A21 * inv_det, im.m22 = A22 * inv_det, im.m23 = A23 * inv_det, im.m24 = A24 * inv_det;
    im.m31 = A31 * inv_det, im.m32 = A32 * inv_det, im.m33 = A33 * inv_det, im.m34 = A34 * inv_det;
    im.m41 = A41 * inv_det, im.m42 = A42 * inv_det, im.m43 = A43 * inv_det, im.m44 = A44 * inv_det;
    return im;
}
