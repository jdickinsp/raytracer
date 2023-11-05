#include <raycast.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far, int skip_index) {
    float closet_hit = INFINITY;
    hit_info->hit = -1;
    ObjectNode *node = objects->head;
    int hit_index = 0;
    for (; node != NULL; node = node->next) {
        if (skip_index >= 0 && skip_index == hit_index) {
            continue;
        }
        float hit = object_intersection(node->current, node->type, ray, hit_info);
        if (hit > 0 && hit > near && hit < far && hit < closet_hit) {
            closet_hit = hit;
            hit_info->node = node;
            hit_info->hit_index = hit_index;
        }
        hit_index++;
    }
    if (closet_hit != INFINITY) {
        if (hit_info->node->type != MeshInfoType) {
            hit_info->hit = closet_hit;
            Vec3 hit_position = ray_normal_at(ray, closet_hit);
            hit_info->position = hit_position;
            Material *material = object_material(hit_info->node->current, hit_info->node->type);
            hit_info->material = material;
            if (hit_info->node->type == SphereType) {
                Sphere s = hit_info->node->current->sphere;
                Vec3 hit_normal = vec3_norm(vec3_sub(hit_position, s.position));
                bool front_face = dot_product(ray->direction, hit_normal) < 0.0;
                hit_info->front_face = front_face;
                hit_info->normal = front_face ? hit_normal : vec3_neg(hit_normal);
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
                hit_info->front_face = true;
                Vec3 hit_normal =
                    vec3_norm(object_normal_at(hit_info->node->current, hit_info->node->type, hit_position));
                hit_info->normal = vec3_neg(hit_normal);
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
            MeshInfo *mesh_info = &hit_info->node->current->mesh_info;
            Mesh *mesh = mesh_info->mesh;
            Vec3 closest_barycentric = (Vec3){hit_info->iu, hit_info->iv, (1 - (hit_info->iu + hit_info->iv))};
            int hit_index = hit_info->tri_index;
            int i, j, k;
            i = hit_index * 3 + 1;
            j = hit_index * 3 + 2;
            k = hit_index * 3;
            // Vec3 hit_normal = vec3_add(vec3_add(vec3_mul(mesh->vertex_normals[i], closest_barycentric.x),
            //                                     vec3_mul(mesh->vertex_normals[j], closest_barycentric.y)),
            //                            vec3_mul(mesh_info->mesh->vertex_normals[k], closest_barycentric.z));
            // using a face normal will not have shading
            Vec3 hit_normal = mesh->face_normals[hit_index];
            hit_info->position = vec3_add(ray_normal_at(hit_info->ray, closet_hit), mesh_info->offset);
            hit_info->hit = closet_hit;
            bool front_face = dot_product(vec3_neg(hit_info->ray->direction), hit_normal) < 0.0;
            hit_info->front_face = front_face;
            hit_info->normal = front_face ? hit_normal : vec3_neg(hit_normal);
            hit_info->material = mesh_info->material;
            float u = (mesh->texture_uv[i].x * closest_barycentric.x + mesh->texture_uv[j].x * closest_barycentric.y +
                       mesh->texture_uv[k].x * closest_barycentric.z) /
                      3.f;
            float v = (mesh->texture_uv[i].y * closest_barycentric.x + mesh->texture_uv[j].y * closest_barycentric.y +
                       mesh->texture_uv[k].y * closest_barycentric.z) /
                      3.f;
            hit_info->u = u;
            hit_info->v = v;

            if (mesh_info->material->checkerboard) {
                hit_info->color = texture_checkboard(u, v, mesh_info->material->scale, &mesh_info->material->color);
            } else if (mesh_info->material->texture != NULL) {
                int iu = (int)(u * mesh_info->material->texture->width) % mesh_info->material->texture->width;
                int iv = (int)(v * mesh_info->material->texture->height) % mesh_info->material->texture->height;
                hit_info->color = texture_pixel_data(mesh_info->material->texture, iu, iv);
            } else {
                hit_info->color = closest_barycentric;
            }
        }
        hit_info->ray = ray;
        return true;
    }
    return false;
}
