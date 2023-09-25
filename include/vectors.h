#ifndef VECTORS_H
#define VECTORS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y, z, w;
} Vec4;

inline Vec3 vec3_norm(Vec3 vec) {
    float mag = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vec3 norm_vec = {.x = vec.x / mag, .y = vec.y / mag, .z = vec.z / mag};
    return norm_vec;
}

inline Vec3 vec3_neg(Vec3 vec) {
    Vec3 negative_vec = {.x = -vec.x, .y = -vec.y, .z = -vec.z};
    return negative_vec;
}

inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
    return c;
}

inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
    return c;
}

inline Vec3 vec3_mul(Vec3 a, float s) {
    Vec3 c = {.x = a.x * s, .y = a.y * s, .z = a.z * s};
    return c;
}

inline Vec3 vec3_component_mul(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
    return c;
}

inline Vec3 vec3_clip_max(Vec3 a, float clip) {
    Vec3 c = {.x = min(a.x, clip), .y = min(a.y, clip), .z = min(a.z, clip)};
    return c;
}

inline float dot_product(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vec3 cross_product(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.y * b.z - a.z * b.y, .y = a.z * b.x - a.x * b.z, .z = a.x * b.y - a.y * b.x};
    return c;
}

inline Vec3 vec3_create(float x, float y, float z) {
    Vec3 c = {x, y, z};
    return c;
}

inline Vec3 vec3_empty() { return vec3_create(0, 0, 0); }

inline float vec3_length_squared(Vec3 a) { return a.x * a.x + a.y * a.y + a.z * a.z; }

inline float vec3_mag(Vec3 a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }

void vec3_debug_print(Vec3 v);
Vec4 vec4_from_vec3(Vec3 a, float w);
Vec3 vec3_from_vec4(Vec4 a);

Vec3 vec3_rand(float low, float high);
Vec3 rand_in_unit_sphere(void);
Vec3 rand_on_hemisphere(Vec3 normal);
Vec3 random_unit_vector(void);
bool vec3_near_zero(Vec3 v);
Vec3 reflect(Vec3 v, Vec3 n);
Vec3 refract(Vec3 uv, Vec3 n, float refraction_ratio);

#endif
