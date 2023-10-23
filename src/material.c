#include <material.h>

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction) {
    Material *material = malloc(sizeof(Material));
    material->albedo = albedo;
    material->Kd = Kd;
    material->Ks = Ks;
    material->reflective = reflective;
    material->color = color;
    material->index_of_refraction = index_of_refraction;
    material->checkerboard = false;
    material->texture = NULL;
    material->scale = 0.1f;
    return material;
}

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
        printf("vec3_near_zero\n");
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
    return true;
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