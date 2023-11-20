#include <ray_trace.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far, int skip_index);

const float BIAS = 1e-3;

float fresnel(Vec3 *I, Vec3 *N, float ior) {
    float dot = dot_product(*I, *N);
    float cosi = clamp_range(dot, -1.f, 1.f);
    float etai = 1;
    float etat = ior;
    float kr = 0;
    if (cosi > 0) {
        float temp = etai;
        etai = etat;
        etat = temp;
    }
    float sint = (etai / etat) * sqrtf(fmax(0.f, 1 - cosi * cosi));
    if (sint >= 1) {
        kr = 1;
    } else {
        float cost = sqrtf(fmax(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2.f;
    }
    return kr;
}

Vec3 reflect_vec(Vec3 *I, Vec3 *N) { return vec3_sub(*I, vec3_mul(*N, 2 * dot_product(*I, *N))); }

Vec3 refract_vec(Vec3 *I, Vec3 *N, float ior) {
    float dot = dot_product(*I, *N);
    float cosi = clamp_range(dot, -1.f, 1.f);
    float etai = 1;
    float etat = ior;
    Vec3 n = vec3_copy(*N);
    if (cosi < 0) {
        cosi = -cosi;
    } else {
        float temp = etai;
        etai = etat;
        etat = temp;
        n = vec3_neg(*N);
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    if (k < 0) {
        return vec3_empty();
    }
    return vec3_add(vec3_mul(*I, eta), vec3_mul(n, eta * cosi - sqrtf(k)));
}

void phong_shading(Ray *ray, ObjectList *obj_list, ObjectList *lights, HitInfo *hit_info, Vec3 *attenuation) {
    ObjectNode *light_node = lights->head;
    Vec3 diffuse = {0, 0, 0};
    Vec3 specular = {0, 0, 0};
    Vec3 hitcolor = {0, 0, 0};
    for (; light_node != NULL; light_node = light_node->next) {
        Vec3 color = hit_info->color;
        // calculate illumination
        Vec3 light_dir;
        Vec3 light_intensity;
        float light_distance;
        if (light_node->type == PointLightType) {
            light_dir = vec3_sub(hit_info->position, light_node->current->point_light.position);
            float r2 = vec3_mag(light_dir);
            light_distance = sqrt(r2);
            light_dir = vec3_mul(light_dir, (1.f / light_distance));
            light_intensity = vec3_mul(light_node->current->point_light.color,
                                       light_node->current->point_light.intensity / (4.f * M_PI * r2));

        } else if (light_node->type == DirectionalLightType) {
            light_dir = light_node->current->directional_light.direction;
            light_intensity = vec3_mul(light_node->current->directional_light.color,
                                       light_node->current->directional_light.intensity);
            light_distance = INFINITY;
        }
        Vec3 shadow_origin = vec3_mul(vec3_add(hit_info->position, hit_info->normal), 1.f + BIAS);
        Ray kshadow = {shadow_origin, vec3_neg(ray->direction), INFINITY};
        HitInfo shadow_hit;
        bool vis = !detect_ray_hits(&kshadow, obj_list, &shadow_hit, 1e-3, INFINITY, hit_info->hit_index);
        // bool vis = true;
        Vec3 color_intensity = vec3_component_mul(color, light_intensity);

        // float angle = 45 * (180.f / M_PI);
        // float s = hit_info->u * cos(angle) - hit_info->v * sin(angle);
        // float t = hit_info->v * cos(angle) + hit_info->u * sin(angle);
        // float scaleS = 5.f;
        // float scaleT = 5.f;
        // float pattern = (s * scaleS - floorf(s * scaleS)) < 0.5f;
        // Vec3 d_color =
        //     vec3_mul(color_intensity, vis * pattern * fmax(0.f, dot_product(hit_info->normal, vec3_neg(light_dir))));
        // Vec3 color_intensity = vec3_component_mul(color, light_intensity);
        // Vec3 s_color = vec3_mul(light_intensity, vis * fmax(0.f, dot_product(hit_info->normal,
        // vec3_neg(light_dir)))); hitcolor = vec3_add(hitcolor, light_intensity);

        // // Vec3 light_dir = light_direction(light_node->current, light_node->type, hit_info->position);
        // // float diffuse_intensity = light_intensity(light_node->current, light_node->type);
        // // Vec3 color = object_color(hit_info->node->current, hit_info->node->type);

        // // Vec3 d_color =
        // //     vec3_mul(vec3_mul(color_intensity, vis * fmax(0.f, dot_product(hit_info->normal,
        // vec3_neg(light_dir)))),
        // //              (hit_info->material->albedo / M_PI));

        Vec3 d_color = vec3_mul(color_intensity, vis * hit_info->material->albedo *
                                                     fmax(0.f, dot_product(hit_info->normal, vec3_neg(light_dir))));
        diffuse = vec3_add(diffuse, d_color);

        // specular
        Vec3 R = vec3_norm(reflect_vec(&light_dir, &hit_info->normal));
        int n = 5;
        // float mag = vec3_mag(light_dir);
        // Vec3 light_intensity = vec3_mul(light_intensity, 1 / (4 * M_PI * mag));
        Vec3 s_color = vec3_mul(color_intensity, vis * pow(fmax(0.f, dot_product(R, vec3_neg(ray->direction))), n));
        specular = vec3_add(specular, s_color);
    }
    *attenuation = vec3_clip_max(
        vec3_add(vec3_mul(diffuse, hit_info->material->Kd), vec3_mul(specular, hit_info->material->Ks)), 1.0f);
    // *attenuation = diffuse;
    // *attenuation = vec3_mul(vec3_add(hit_info->normal, (Vec3){1, 1, 1}), 0.5f);
    // vec3_debug_print(hit_info->normal);
}

// trace another ray at intersection point
void reflection_color_shading(HitInfo *hit_info, Ray *ray, Vec3 *attenuation) {
    Vec3 R = reflect_vec(&ray->direction, &hit_info->normal);
    *attenuation = vec3_norm(R);
}

// cast a reflection and refraction ray at intersection
void reflection_and_refraction_shading(Scene *scene, HitInfo *hit_info, Ray *ray, int depth, Vec3 *attenuation) {
    Vec3 refraction_color = {0, 0, 0};
    Vec3 reflection_color = {0, 0, 0};
    float kr = fresnel(&ray->direction, &hit_info->normal, hit_info->material->index_of_refraction);
    bool outside = dot_product(ray->direction, hit_info->normal) < 0;
    Vec3 bias = vec3_mul(hit_info->normal, BIAS);
    if (kr < 1) {
        Vec3 refraction_dir = refract_vec(&ray->direction, &hit_info->normal, hit_info->material->index_of_refraction);
        refraction_dir = vec3_norm(refraction_dir);
        Vec3 refraction_ray_origin = outside ? vec3_sub(hit_info->position, bias) : vec3_add(hit_info->position, bias);
        Ray refraction_ray = {refraction_ray_origin, refraction_dir, INFINITY};
        refraction_color = ray_trace_color(scene, &refraction_ray, depth - 1);
    }
    Vec3 reflection_dir = reflect_vec(&ray->direction, &hit_info->normal);
    reflection_dir = vec3_norm(reflection_dir);
    Vec3 reflection_ray_origin = outside ? vec3_add(hit_info->position, bias) : vec3_sub(hit_info->position, bias);
    Ray reflection_ray = {reflection_ray_origin, reflection_dir, INFINITY};
    reflection_color = ray_trace_color(scene, &reflection_ray, depth - 1);

    Vec3 color = vec3_add(vec3_mul(reflection_color, kr), vec3_mul(refraction_color, (1 - kr)));
    *attenuation = vec3_clip_max(vec3_add(*attenuation, color), 1.0f);
}

Vec3 ray_trace_color(Scene *scene, Ray *ray, int depth) {
    if (depth <= 0) return vec3_empty();
    Vec3 hit_color = {0, 0, 0};
    HitInfo hit_info;
    ObjectList *objects = scene->objects;
    ObjectList *lights = scene->lights;
    bool hit = detect_ray_hits(ray, objects, &hit_info, 1e-3, INFINITY, -1);
    if (hit) {
        // hit_color = vec3_mul(hit_info.color, dot_product(hit_info.normal, vec3_neg(ray->direction)));
        if (hit_info.material->index_of_refraction > 0 && hit_info.material->reflective == true) {  // transparent
            reflection_and_refraction_shading(scene, &hit_info, ray, depth - 1, &hit_color);
        } else if (hit_info.material->reflective == true) {  // reflective
            Vec3 reflect_dir;
            reflection_color_shading(&hit_info, ray, &reflect_dir);
            Vec3 reflect_point = vec3_add(hit_info.position, hit_info.normal);
            Ray reflection_ray = {reflect_point, reflect_dir, INFINITY};
            Vec3 color = ray_trace_color(scene, &reflection_ray, depth - 1);
            hit_color = vec3_add(hit_color, vec3_mul(color, 0.8f));
        } else {  // diffuse, uses phong shading
            phong_shading(ray, objects, lights, &hit_info, &hit_color);
        }
        return hit_color;
    }
    Vec3 unit_dir = vec3_norm(ray->direction);
    float a = 0.5 * (unit_dir.y + 1.0);
    return vec3_add(vec3_mul(vec3_create(1.0, 1.0, 1.0), 1.0 - a), vec3_mul(vec3_create(0.5, 0.7, 1.0), a));
}
