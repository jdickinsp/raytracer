#include <object_mesh.h>

#define USE_BVH

MeshInfo *object_mesh_create(Mesh *mesh, Material *material, Vec3 offset) {
    MeshInfo *object_mesh = malloc(sizeof(MeshInfo));
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
    Primitives *primitives = bvh_prepare_data(triangles, n_size);
    BVHNode *bvh = bvh_build_tree(primitives);
    object_mesh->bvh = bvh;
    object_mesh->primitives = primitives;
    object_mesh->offset = offset;
    return object_mesh;
}

float object_mesh_intersection(MeshInfo *object_mesh, Ray *ray, HitInfo *hit_info) {
    Mesh *mesh = object_mesh->mesh;
    float closet_hit = INFINITY;
    int hit_index = 0;
    Vec3 barycentric;
    Vec3 closest_barycentric;
    Ray ray_offset = (Ray){vec3_sub(ray->origin, object_mesh->offset), ray->direction, ray->t};
    // // check if in boundary sphere
    // HitInfo sphere_hit;
    // sphere_intersection(&object_mesh->bounding_sphere->sphere, &ray_offset, &sphere_hit);
    // if (sphere_hit.hit > 0) {
    //     return -1;
    // }

    // add BVH intersection
#ifdef USE_BVH
    BVHitInfo bv_hit = {-1, NULL, false};
    bvh_raycast(object_mesh, &ray_offset, &bv_hit);
    if (bv_hit.has_hit == true && bv_hit.hit < ray->t) {
        closet_hit = bv_hit.hit;
        hit_index = bv_hit.index;
        closest_barycentric = bv_hit.barycentric;
        ray->t = bv_hit.hit;
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
        hit_info->iu = bv_hit.barycentric.x;
        hit_info->iv = bv_hit.barycentric.y;
        hit_info->tri_index = hit_index;
        hit_info->hit = closet_hit;
        hit_info->ray = &ray_offset;
    }
    return closet_hit;
}

void bvh_raycast(MeshInfo *mesh_info, Ray *ray, BVHitInfo *bv_hit) {
    float tmin = INFINITY;
    float t = 0.f;
    int s = 0;
    BVHNode *root = mesh_info->bvh;
    Primitives *primitives = mesh_info->primitives;
    BVHNode **stack[BVH_MAX_STACK_SIZE];
    stack[++s] = &root;
    BVHNode *current;
    while (s > 0) {
        current = *stack[s--];
        if (!bounding_box_intersection(current->aabb, ray, &t)) continue;
        if (current->is_leaf == true) {
            int tri_index = current->data;
            BVTriangle triangle = primitives->triangles[tri_index];
            float hit_dist =
                mesh_triangle_intersection(ray, triangle.v1, triangle.v2, triangle.v3, &bv_hit->barycentric);
            if (hit_dist > 0 && t <= tmin) {
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
    Vec3 color = {0, 0, 0};
    Material material = {0, 0, 0, false, color};
    Sphere *sphere = sphere_create(center, radius, &material);
    bs->max_bounds = max_bounds;
    bs->min_bounds = min_bounds;
    bs->sphere = *sphere;
    return bs;
}
