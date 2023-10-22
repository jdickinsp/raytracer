#include <object_mesh.h>

#define USE_BVH

ObjectMesh *object_mesh_create(Mesh *mesh, Material *material, Vec3 offset) {
    ObjectMesh *object_mesh = malloc(sizeof(ObjectMesh));
    Vec3 min_bounds, max_bounds, center;
    mesh_boundaries(mesh, &min_bounds, &max_bounds, &center);
    BoundingSphere *bounding_sphere = bounding_sphere_create(max_bounds, min_bounds, center);
    object_mesh->mesh = mesh;
    object_mesh->bounding_sphere = bounding_sphere;
    object_mesh->material = material;
    // create bounding volume hierarchy
    int n_size = mesh->vertex_count / 3;
    BVTriangle *triangles = malloc(sizeof(BVTriangle) * n_size);
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n * 3;
        int j = n * 3 + 1;
        int k = n * 3 + 2;
        triangles[n] = (BVTriangle){mesh->vertices[i], mesh->vertices[j], mesh->vertices[k]};
    }
    Primatives *primatives = bvh_prepare_data(triangles, n_size);
    BVHNode *bvh = bvh_build_tree(primatives);
    object_mesh->bvh = bvh;
    object_mesh->primatives = primatives;
    object_mesh->offset = offset;
    // bvh_pprint(bvh);
    return object_mesh;
}

float object_mesh_intersection(ObjectMesh *object_mesh, Ray *ray, HitInfo *hit_info) {
    Mesh *mesh = object_mesh->mesh;
    float closet_hit = INFINITY;
    int hit_index = 0;
    Vec3 barycentric;
    Vec3 closest_barycentric;
    // // check if in boundary sphere
    // HitInfo sphere_hit;
    // sphere_intersection(&object_mesh->bounding_sphere->sphere, ray, &sphere_hit);
    // if (sphere_hit.hit > 0) {
    //     return -1;
    // }

    // add BVH intersection
#ifdef USE_BVH
    BVHitInfo bv_hit = {-1, NULL, false};
    bvh_raycast(object_mesh, ray, &bv_hit);
    if (bv_hit.has_hit == true) {
        closet_hit = bv_hit.hit;
        hit_index = bv_hit.index;
        closest_barycentric = bv_hit.barycentric;
    }
#else
    // printf("hit_info: %i, %f\n", bvh_hit_info.index, bvh_hit_info.t);
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n * 3;
        int j = n * 3 + 1;
        int k = n * 3 + 2;
        float hit =
            mesh_triangle_intersection(ray, mesh->vertices[i], mesh->vertices[j], mesh->vertices[k], &barycentric);
        if (hit > 0 && hit < closet_hit) {
            // vec3_debug_print(mesh->vertices[i]);
            closet_hit = hit;
            hit_index = n;
            closest_barycentric = barycentric;
            // printf("tri_index: %i\n", n);
        }
    }
#endif
    if (closet_hit != INFINITY) {
        int i, j, k;
        i = hit_index * 3 + 1;
        j = hit_index * 3 + 2;
        k = hit_index * 3;
        // Vec3 hit_normal = vec3_add(vec3_add(vec3_mul(mesh->vertex_normals[i], closest_barycentric.x),
        //                                     vec3_mul(mesh->vertex_normals[j], closest_barycentric.y)),
        //                            vec3_mul(mesh->vertex_normals[k], closest_barycentric.z));
        // hit_normal = vec3_norm(hit_normal);
        // using a face normal will just mean it's not smooth
        Vec3 hit_normal = mesh->face_normals[hit_index];
        hit_info->position = ray_normal_at(ray, closet_hit);
        hit_info->hit = closet_hit;
        bool front_face = dot_product(vec3_neg(ray->direction), hit_normal) < 0.0;
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
        // int iu = (int)(u * object_mesh->material->texture->width) % object_mesh->material->texture->width;
        // int iv = (int)(v * object_mesh->material->texture->height) % object_mesh->material->texture->height;
        // hit_info->color = texture_pixel_data(object_mesh->material->texture, iu, iv);
        hit_info->color = texture_checkboard(u, v, 0.01f);
        // hit_info->color = closest_barycentric;
    }
    return closet_hit;
}

void bvh_raycast(ObjectMesh *obj_mesh, Ray *ray, BVHitInfo *bv_hit) {
    float tmin = INFINITY;
    float t = 0.f;
    int s = 0;
    BVHNode *root = obj_mesh->bvh;
    Primatives *primatives = obj_mesh->primatives;
    BVHNode **stack[BVH_MAX_STACK_SIZE];
    stack[++s] = &root;
    BVHNode *current;
    // Ray ray_offset = (Ray){ray->origin, ray->direction, ray->t};
    // ray_offset.origin = vec3_sub(ray->origin, obj_mesh->offset);
    while (s > 0) {
        current = *stack[s--];
        // AABoundingBox box_offset = (AABoundingBox){vec3_sub(current->aabb->min, obj_mesh->offset),
        //                                            vec3_sub(current->aabb->min, obj_mesh->offset)};
        if (!bounding_box_intersection(current->aabb, ray, &t)) continue;
        if (current->is_leaf == true) {
            int tri_index = current->data;
            BVTriangle triangle = primatives->triangles[tri_index];
            float hit_dist =
                mesh_triangle_intersection(ray, triangle.v1, triangle.v2, triangle.v3, &bv_hit->barycentric);
            if (hit_dist > 0 && t <= tmin) {
                // ray->t = ray_offset.t;
                bv_hit->index = current->data;
                bv_hit->ray = ray;
                bv_hit->has_hit = true;
                bv_hit->hit = t;
                tmin = t;
            }
        } else {
            if (current->left) {
                stack[++s] = &current->left;
            }
            if (current->right) {
                stack[++s] = &current->right;
            }
        }
    }
}

void mesh_set_face_normals(Mesh *mesh) {
    Vec3 *face_normals = malloc(sizeof(Vec3) * mesh->vertex_count / 3);
    float *directions = malloc(sizeof(float) * mesh->vertex_count / 3);
    int s = 0;
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n + s;
        int j = n + s + 1;
        int k = n + s + 2;
        Vec3 normal = vec3_triangle_normal_at(mesh->vertices[i], mesh->vertices[j], mesh->vertices[k]);
        float d = -dot_product(normal, mesh->vertices[k]);
        face_normals[n] = normal;
        directions[n] = d;
        s += 2;
    }
    mesh->face_normals = face_normals;
    mesh->directions = directions;
}

BoundingSphere *bounding_sphere_create(Vec3 max_bounds, Vec3 min_bounds, Vec3 center) {
    float radius = max(max(max_bounds.x - center.x, max_bounds.y - center.y), max_bounds.z - center.z) * 2;
    printf("radius: %f\n", radius);
    BoundingSphere *bs = malloc(sizeof(BoundingSphere));
    // TODO remove color and material
    Vec3 color = {0, 0, 0};
    Material material = {0, 0, 0, false, color};
    Sphere *sphere = sphere_create(center, radius, &material);
    bs->max_bounds = max_bounds;
    bs->min_bounds = min_bounds;
    bs->sphere = *sphere;
    return bs;
}
