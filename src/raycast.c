#include <raycast.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far) {
    float closet_hit = INFINITY;
    hit_info->hit = -1;
    ObjectNode *node = objects->head;
    for (; node != NULL; node = node->next) {
        float hit = object_intersection(node->current, node->type, ray, hit_info);
        if (hit > 0 && hit > near && hit < far && hit < closet_hit) {
            closet_hit = hit;
            hit_info->node = node;
        }
    }
    if (closet_hit != INFINITY) {
        if (hit_info->node->type != ObjectMeshType) {
            hit_info->hit = closet_hit;
            Vec3 hit_position = ray_normal_at(ray, closet_hit);
            Vec3 hit_normal = vec3_norm(object_normal_at(hit_info->node->current, hit_info->node->type, hit_position));
            hit_info->position = hit_position;
            bool front_face = dot_product(ray->direction, hit_normal) < 0.0;
            hit_info->front_face = front_face;
            hit_info->normal = front_face ? hit_normal : vec3_neg(hit_normal);
            Material *material = object_material(hit_info->node->current, hit_info->node->type);
            hit_info->material = material;
            if (hit_info->node->type == SphereType) {
                Sphere s = hit_info->node->current->sphere;
                Vec3 outward_normal = vec3_mul(vec3_sub(hit_info->position, s.position), (1 / s.radius));
                Vec2 uv = sphere_uv_texture_coord(outward_normal);
                hit_info->u = uv.x;
                hit_info->v = uv.y;
                if (s.material->texture != NULL) {
                    int i = uv.x * s.material->texture->width;
                    int j = uv.y * s.material->texture->height;
                    hit_info->color = texture_pixel_data(s.material->texture, i, j);
                } else if (s.material->checkerboard == true) {
                    hit_info->color =
                        texture_checkboard(uv.x, uv.y, hit_info->material->scale, &hit_info->material->color);
                } else {
                    hit_info->color = object_color(hit_info->node->current, hit_info->node->type);
                }
            } else if (hit_info->node->type == PlaneType) {
                Plane plane = hit_info->node->current->plane;
                Vec2 uv = {fmod(hit_position.x, 1), fmod(hit_position.z, 1)};
                hit_info->u = uv.x;
                hit_info->v = uv.y;
                if (plane.material->checkerboard == true) {
                    hit_info->color =
                        texture_checkboard(uv.x, uv.y, hit_info->material->scale, &hit_info->material->color);
                } else {
                    hit_info->color = object_color(hit_info->node->current, hit_info->node->type);
                }

            } else {
                hit_info->color = object_color(hit_info->node->current, hit_info->node->type);
            }
        } else {
            ObjectMesh *object_mesh = &hit_info->node->current->object_mesh;
            Mesh *mesh = object_mesh->mesh;
            Vec3 closest_barycentric = (Vec3){hit_info->iu, hit_info->iv, (1 - (hit_info->iu + hit_info->iv))};
            int hit_index = hit_info->tri_index;
            int i, j, k;
            i = hit_index * 3 + 1;
            j = hit_index * 3 + 2;
            k = hit_index * 3;
            Vec3 hit_normal = vec3_add(vec3_add(vec3_mul(mesh->vertex_normals[i], closest_barycentric.x),
                                                vec3_mul(mesh->vertex_normals[j], closest_barycentric.y)),
                                       vec3_mul(object_mesh->mesh->vertex_normals[k], closest_barycentric.z));
            // using a face normal will not have shading
            // Vec3 hit_normal = mesh->face_normals[hit_index];
            hit_info->position = vec3_add(ray_normal_at(hit_info->ray, closet_hit), object_mesh->offset);
            hit_info->hit = closet_hit;
            bool front_face = dot_product(vec3_neg(hit_info->ray->direction), hit_normal) < 0.0;
            hit_info->front_face = front_face;
            hit_info->normal = front_face ? hit_normal : vec3_neg(hit_normal);
            hit_info->material = object_mesh->material;
            float u = (mesh->texture_uv[i].x * closest_barycentric.x + mesh->texture_uv[j].x * closest_barycentric.y +
                       mesh->texture_uv[k].x * closest_barycentric.z) /
                      3.f;
            float v = (mesh->texture_uv[i].y * closest_barycentric.x + mesh->texture_uv[j].y * closest_barycentric.y +
                       mesh->texture_uv[k].y * closest_barycentric.z) /
                      3.f;
            hit_info->u = u;
            hit_info->v = v;

            if (object_mesh->material->checkerboard) {
                hit_info->color = texture_checkboard(u, v, object_mesh->material->scale, &object_mesh->material->color);
            } else if (object_mesh->material->texture != NULL) {
                int iu = (int)(u * object_mesh->material->texture->width) % object_mesh->material->texture->width;
                int iv = (int)(v * object_mesh->material->texture->height) % object_mesh->material->texture->height;
                hit_info->color = texture_pixel_data(object_mesh->material->texture, iu, iv);
            } else {
                hit_info->color = closest_barycentric;
            }
        }
        return true;
    }
    return false;
}

// Vec3 ray_trace_color(Ray *ray, Scene *scene) {
//     if (scene->lights == NULL) return vec3_empty();
//     RenderOptions *options = scene->render_options;
//     Vec3 hit_color = {0, 0, 0};
//     HitInfo hit_info;
//     ObjectList *objects = scene->objects;
//     ObjectList *lights = scene->lights;
//     Sphere *sphere = (Sphere *)objects->head->current;
//     bool has_hit = detect_ray_hits(ray, objects, &hit_info, 1e-3, INFINITY);
//     if (has_hit) {
//         if (options->rendering_type == WHITTED_RAY_TRACING) {
//             hit_color = vec3_mul(object_color(hit_info.node->current, hit_info.node->type),
//                                  dot_product(hit_info.normal, vec3_neg(ray->direction)));
//         } else if (options->rendering_type == RAY_TRACING_2) {
//             // Phong shading
//             ObjectNode *light_node = lights->head;
//             Material *material = object_material(hit_info.node->current, hit_info.node->type);
//             Vec3 diffuse = {0, 0, 0};
//             Vec3 specular = {0, 0, 0};
//             // cast shadow
//             float shadow_bias = 1e-4;
//             HitInfo shadow_info;
//             Vec3 shadow_origin = vec3_add(hit_info.position, vec3_mul(hit_info.normal, shadow_bias));
//             for (; light_node != NULL; light_node = light_node->next) {
//                 Vec3 light_dir = light_direction(light_node->current, light_node->type, hit_info.position);
//                 float diffuse_intensity = light_intensity(light_node->current, light_node->type);
//                 // Vec3 shadow_light = light_dir;
//                 // Ray shadow_ray = { shadow_origin, shadow_light };
//                 // bool shadow_hit = detect_ray_hits(&shadow_ray, objects, &shadow_info, 1e-3, INFINITY);
//                 if (true) {
//                     // diffuse
//                     Vec3 color_intensity =
//                         vec3_mul(object_color(hit_info.node->current, hit_info.node->type), diffuse_intensity);
//                     Vec3 d_color =
//                         vec3_mul(vec3_mul(color_intensity, max(0.f, dot_product(hit_info.normal, light_dir))),
//                                  (material->albedo / M_PI));
//                     diffuse = vec3_add(diffuse, d_color);
//                     // specular
//                     Vec3 reflect =
//                         vec3_sub(light_dir, vec3_mul(hit_info.normal, 2 * dot_product(hit_info.normal, light_dir)));
//                     int n = 4;
//                     float mag = vec3_mag(light_dir);
//                     Vec3 light_intensity = vec3_mul(color_intensity, 1 / (4 * M_PI * mag));
//                     Vec3 s_color = vec3_mul(light_intensity, pow(max(0.f, dot_product(reflect, light_dir)), n));
//                     specular = vec3_add(specular, s_color);
//                 }
//             }
//             hit_color =
//                 vec3_clip_max(vec3_add(vec3_mul(diffuse, material->Kd), vec3_mul(specular, material->Ks)), 1.0f);
//         } else if (options->rendering_type == RAY_TRACING_3) {
//             // cast shadow
//             float shadow_bias = 1e-4;
//             HitInfo shadow_info;
//             Vec3 shadow_origin = vec3_add(hit_info.position, vec3_mul(hit_info.normal, shadow_bias));
//             ObjectNode *light_node = lights->head;
//             for (; light_node != NULL; light_node = light_node->next) {
//                 Vec3 light_dir = light_direction(light_node->current, light_node->type, hit_info.position);
//                 float diffuse_intensity = light_intensity(light_node->current, light_node->type);
//                 // Ray shadow_ray = { shadow_origin, light_dirn };
//                 // bool shadow_hit = detect_ray_hits(&shadow_ray, objects, &shadow_info, 1e-3, INFINITY);
//                 if (true) {
//                     Material *material = object_material(hit_info.node->current, hit_info.node->type);
//                     Vec3 shadow_color = vec3_mul(object_color(hit_info.node->current, hit_info.node->type),
//                                                  (material->albedo / M_PI) * diffuse_intensity *
//                                                      max(0.f, dot_product(hit_info.normal, light_dir)));
//                     Vec3 color = vec3_clip_max(vec3_mul(vec3_add(hit_color, shadow_color), 0.5f), 1.f);
//                     hit_color = vec3_add(hit_color, color);
//                 }
//             }
//         }
//     }
//     return hit_color;
// }

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
            hit_color = vec3_mul(object_color(hit_info.node->current, hit_info.node->type),
                                 dot_product(hit_info.normal, vec3_neg(ray->direction)));
        }
        return hit_color;
    }
    Vec3 unit_dir = vec3_norm(ray->direction);
    float a = 0.5 * (unit_dir.y + 1.0);
    return vec3_add(vec3_mul(vec3_create(1.0, 1.0, 1.0), 1.0 - a), vec3_mul(vec3_create(0.5, 0.7, 1.0), a));
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
