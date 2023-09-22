#ifndef VECTORS_H
#define VECTORS_H

#include <stdlib.h>
#include <math.h>
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

Vec3 vec3_norm(Vec3 vec);
Vec3 vec3_neg(Vec3 vec);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 a, float s);
Vec3 vec3_component_mul(Vec3 a, Vec3 b);
float vec3_length_squared(Vec3 a);
float vec3_mag(Vec3 a);
Vec3 vec3_clip_max(Vec3 a, float clip);
float dot_product(Vec3 a, Vec3 b);
Vec3 cross_product(Vec3 a, Vec3 b);
Vec3 vec3_create(float x, float y, float z);
Vec3 vec3_empty();
Vec3 vec3_rand(float low, float high);

void vec3_debug_print(Vec3 v);
Vec4 vec4_from_vec3(Vec3 a, float w);
Vec3 vec3_from_vec4(Vec4 a);

Vec3 rand_in_unit_sphere();
Vec3 rand_on_hemisphere(Vec3 normal);
Vec3 random_unit_vector();
bool vec3_near_zero(Vec3 v);
Vec3 reflect(Vec3 v, Vec3 n);
Vec3 refract(Vec3 uv, Vec3 n, float refraction_ratio);

#endif