#include <vectors.h>

Vec3 vec3_norm(Vec3 vec) {
    float mag = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vec3 norm_vec = {.x = vec.x / mag, .y = vec.y / mag, .z = vec.z / mag};
    return norm_vec;
}

Vec3 vec3_neg(Vec3 vec) {
    Vec3 negative_vec = {.x = -vec.x, .y = -vec.y, .z = -vec.z};
    return negative_vec;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
    return c;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
    return c;
}

Vec3 vec3_mul(Vec3 a, float s) {
    Vec3 c = {.x = a.x * s, .y = a.y * s, .z = a.z * s};
    return c;
}

Vec3 vec3_component_mul(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
    return c;
}

Vec3 vec3_clip_max(Vec3 a, float clip) {
    Vec3 c = {.x = min(a.x, clip), .y = min(a.y, clip), .z = min(a.z, clip)};
    return c;
}

float dot_product(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 cross_product(Vec3 a, Vec3 b) {
    Vec3 c = {.x = a.y * b.z - a.z * b.y, .y = a.z * b.x - a.x * b.z, .z = a.x * b.y - a.y * b.x};
    return c;
}

Vec3 vec3_create(float x, float y, float z) {
    Vec3 c = {x, y, z};
    return c;
}

Vec3 vec3_empty() { return vec3_create(0, 0, 0); }

float vec3_length_squared(Vec3 a) { return a.x * a.x + a.y * a.y + a.z * a.z; }

float vec3_mag(Vec3 a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }

Vec3 vec3_rand(float low, float high) {
    Vec3 v = {rand_range(low, high), rand_range(low, high), rand_range(low, high)};
    return v;
}

void vec3_debug_print(Vec3 v) { printf("[%f %f %f]\n", v.x, v.y, v.z); }

Vec4 vec4_from_vec3(Vec3 a, float w) {
    Vec4 v = {a.x, a.y, a.z, w};
    return v;
}

Vec3 vec3_from_vec4(Vec4 a) {
    Vec3 v = {a.x / a.w, a.y / a.w, a.z / a.w};
    return v;
}

Vec3 rand_in_unit_sphere() {
    while (true) {
        Vec3 p = vec3_rand(-1, 1);
        if (vec3_length_squared(p) < 1) {
            return p;
        }
    }
}

Vec3 rand_on_hemisphere(Vec3 normal) {
    Vec3 on_unit_sphere = vec3_norm(rand_in_unit_sphere());
    if (dot_product(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }
    return vec3_neg(on_unit_sphere);
}

Vec3 random_unit_vector() { return vec3_norm(rand_in_unit_sphere()); }

bool vec3_near_zero(Vec3 v) {
    float epsilon = 1e-8;
    return fabs(v.x) < epsilon && fabs(v.y) < epsilon && fabs(v.z) < epsilon;
}

Vec3 reflect(Vec3 v, Vec3 n) { return vec3_sub(v, vec3_mul(n, dot_product(v, n) * 2)); }

Vec3 refract(Vec3 uv, Vec3 n, float refraction_ratio) {
    float cos_theta = fmin(dot_product(vec3_neg(uv), n), 1.0);
    Vec3 r_out_perpendicular = vec3_mul(vec3_add(uv, vec3_mul(n, cos_theta)), refraction_ratio);
    Vec3 r_out_parallel = vec3_mul(n, -sqrt(fabs(1.0 - vec3_length_squared(r_out_perpendicular))));
    return vec3_add(r_out_perpendicular, r_out_parallel);
}
