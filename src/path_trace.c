#include <path_trace.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far);

bool material_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered) {
    if (mat->index_of_refraction > 0) {
        return dielectric_scatter(mat, ray, hit_info, attenuation, scattered);
    } else if (mat->reflective == true) {
        return metal_scatter(mat, ray, hit_info, attenuation, scattered);
    }
    return lambertian_scatter(mat, ray, hit_info, attenuation, scattered);
}

bool lambertian_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered) {
    Vec3 scatter_dir = vec3_add(hit_info->normal, random_unit_vector());
    if (vec3_near_zero(scatter_dir)) {
        scatter_dir = hit_info->normal;
    }
    *scattered = (Ray){hit_info->position, scatter_dir};
    *attenuation = vec3_mul(hit_info->color, mat->albedo);
    return true;
}

bool metal_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered) {
    Vec3 reflected = reflect(vec3_norm(ray->direction), hit_info->normal);
    float fuzz = 0.3;
    reflected = vec3_add(reflected, vec3_mul(random_unit_vector(), fuzz));
    *scattered = (Ray){hit_info->position, reflected};
    *attenuation = vec3_mul(hit_info->color, mat->albedo);
    return dot_product(scattered->direction, hit_info->normal) > 0;
}

bool dielectric_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered) {
    *attenuation = (Vec3){1, 1, 1};
    float refraction_ratio = hit_info->front_face ? (1.0f / mat->index_of_refraction) : mat->index_of_refraction;
    Vec3 unit_direction = vec3_norm(ray->direction);
    float cos_theta = fmin(dot_product(vec3_neg(unit_direction), hit_info->normal), 1.0f);
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    bool cannot_reflect = refraction_ratio * sin_theta > 1.0f;
    Vec3 direction;
    if (cannot_reflect || reflectance(cos_theta, refraction_ratio) > rand_range(0, 1)) {
        direction = reflect(unit_direction, hit_info->normal);
    } else {
        direction = refract(unit_direction, hit_info->normal, refraction_ratio);
    }
    *scattered = (Ray){hit_info->position, direction};
    return true;
}

float reflectance(float cosine, float ref_index) {
    float r0 = (1 - ref_index) / (1 + ref_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

Vec3 path_trace_color(Scene *scene, Ray *ray, int depth) {
    if (depth <= 0) return vec3_empty();
    HitInfo hit_info;
    ObjectList *objects = scene->objects;
    ObjectList *lights = scene->lights;
    bool hit = detect_ray_hits(ray, objects, &hit_info, 1e-3, INFINITY);
    if (hit) {
        Vec3 attenuation;
        Ray scattered;
        if (material_scatter(hit_info.material, ray, &hit_info, &attenuation, &scattered)) {
            Vec3 color = path_trace_color(scene, &scattered, depth - 1);
            return vec3_component_mul(color, attenuation);
        }
        return (Vec3){0, 0, 0};
    }
    Vec3 unit_dir = vec3_norm(ray->direction);
    float a = 0.5 * (unit_dir.y + 1.0);
    return vec3_add(vec3_mul(vec3_create(1.0, 1.0, 1.0), 1.0 - a), vec3_mul(vec3_create(0.5, 0.7, 1.0), a));
}
