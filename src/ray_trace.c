#include <ray_trace.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far);

bool phong_shading(ObjectList *lights, HitInfo *hit_info, Vec3 *attenuation) {
    ObjectNode *light_node = lights->head;
    Vec3 diffuse = {0, 0, 0};
    Vec3 specular = {0, 0, 0};
    for (; light_node != NULL; light_node = light_node->next) {
        Vec3 light_dir = light_direction(light_node->current, light_node->type, hit_info->position);
        float diffuse_intensity = light_intensity(light_node->current, light_node->type);
        // Vec3 color = object_color(hit_info.node->current, hit_info.node->type);
        Vec3 color = hit_info->color;
        Vec3 color_intensity = vec3_mul(color, diffuse_intensity);
        Vec3 d_color = vec3_mul(vec3_mul(color_intensity, fmax(0.f, dot_product(hit_info->normal, light_dir))),
                                (hit_info->material->albedo / M_PI));
        diffuse = vec3_add(diffuse, d_color);
        // specular
        Vec3 reflect = vec3_sub(light_dir, vec3_mul(hit_info->normal, 2 * dot_product(hit_info->normal, light_dir)));
        int n = 10;
        float mag = vec3_mag(light_dir);
        Vec3 light_intensity = vec3_mul(color_intensity, 1 / (4 * M_PI * mag));
        Vec3 s_color = vec3_mul(light_intensity, pow(max(0.f, dot_product(reflect, light_dir)), n));
        specular = vec3_add(specular, s_color);
    }
    *attenuation = vec3_clip_max(
        vec3_add(vec3_mul(diffuse, hit_info->material->Kd), vec3_mul(specular, hit_info->material->Ks)), 1.0f);
    return true;
}

Vec3 ray_trace_color(Scene *scene, Ray *ray, int depth) {
    if (depth <= 0 || scene->lights == NULL) return vec3_empty();
    Vec3 hit_color = {0, 0, 0};
    HitInfo hit_info;
    ObjectList *objects = scene->objects;
    ObjectList *lights = scene->lights;
    bool has_hit = detect_ray_hits(ray, objects, &hit_info, 1e-3, INFINITY);
    if (has_hit) {
        if (true) {
            Vec3 diffuse = {0, 0, 0};
            Vec3 specular = {0, 0, 0};
            // phong shading
            Vec3 attenuation;
            ObjectNode *light_ = lights->head;
            if (phong_shading(lights, &hit_info, &attenuation)) {
                hit_color = attenuation;
            }
        } else {
            hit_color = vec3_mul(hit_info.color, dot_product(hit_info.normal, vec3_neg(ray->direction)));
        }
        return hit_color;
    }
    Vec3 unit_dir = vec3_norm(ray->direction);
    float a = 0.5 * (unit_dir.y + 1.0);
    return vec3_add(vec3_mul(vec3_create(1.0, 1.0, 1.0), 1.0 - a), vec3_mul(vec3_create(0.5, 0.7, 1.0), a));
}
