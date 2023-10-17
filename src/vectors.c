#include <vectors.h>

void vec3_debug_print(Vec3 v) { printf("[%f %f %f]\n", v.x, v.y, v.z); }

Vec4 vec4_from_vec3(Vec3 a, float w) {
    Vec4 v = {a.x, a.y, a.z, w};
    return v;
}

Vec3 vec3_from_vec4(Vec4 a) {
    Vec3 v = {a.x / a.w, a.y / a.w, a.z / a.w};
    return v;
}

Vec3 vec3_rand(float low, float high) {
    Vec3 v = {rand_range(low, high), rand_range(low, high), rand_range(low, high)};
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

Vec3 vec3_distance(Vec3 a, Vec3 b) {
    Vec3 v;
    v.x = sqrt(powf(a.x - b.x, 2));
    v.y = sqrt(powf(a.y - b.y, 2));
    v.z = sqrt(powf(a.z - b.z, 2));
    return v;
}

float vec3_index_value(Vec3 *v, int index) {
    switch (index) {
        case 0:
            return v->x;
        case 1:
            return v->y;
        case 2:
            return v->z;
        default:
            return -1;
    }
}

Vec3 vec3_triangle_normal_at(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 A = vec3_sub(b, a);
    Vec3 B = vec3_sub(c, a);
    return vec3_norm(cross_product(A, B));
}

float mesh_triangle_intersection(Ray *ray, Vec3 a, Vec3 b, Vec3 c, Vec3 *uvw) {
    Vec3 A = vec3_sub(b, a);
    Vec3 B = vec3_sub(c, a);
    Vec3 normal = cross_product(A, B);

    float n_dot_ray_dir = dot_product(normal, ray->direction);
    if (fabs(n_dot_ray_dir) < 1e-4) {
        return -1;
    }
    float d = -dot_product(normal, a);
    float ti = -(d + dot_product(normal, ray->origin)) / n_dot_ray_dir;
    if (ti < 0) {
        return -1;
    }
    Vec3 Q = vec3_add(ray->origin, vec3_mul(ray->direction, ti));
    Vec3 edge0 = vec3_sub(b, a);
    Vec3 vp0 = vec3_sub(Q, a);
    Vec3 C = cross_product(edge0, vp0);
    if (dot_product(normal, C) < 0) {
        return -1;
    }
    Vec3 edge1 = vec3_sub(c, b);
    Vec3 vp1 = vec3_sub(Q, c);
    C = cross_product(edge1, vp1);
    float u = dot_product(normal, C);
    if (u < 0) {
        return -1;
    }
    Vec3 edge2 = vec3_sub(a, c);
    Vec3 vp2 = vec3_sub(Q, c);
    C = cross_product(edge2, vp2);
    float v = dot_product(normal, C);
    if (v < 0) {
        return -1;
    }
    float denom = dot_product(normal, normal);
    // barycentric coordinates: u + v + w = 1
    uvw->x = u / denom;
    uvw->y = v / denom;
    uvw->z = 1.f - uvw->x - uvw->y;
    return ti;
}