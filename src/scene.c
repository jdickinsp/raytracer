#include <scene.h>

ObjectList *object_list_create() {
    ObjectList *list = malloc(sizeof(ObjectList));
    list->head = NULL;
    list->count = 0;
    return list;
}

void object_list_add(ObjectList *object_list, Object *object, ObjectType type) {
    ObjectNode *next_obj = malloc(sizeof(ObjectNode));
    next_obj->current = object;
    next_obj->type = type;
    next_obj->next = NULL;
    ObjectNode *node = object_list->head;
    if (node == NULL) {
        node = next_obj;
        object_list->head = node;
    } else {
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = next_obj;
    }
    object_list->count = object_list->count + 1;
}

float object_intersection(Object *object, ObjectType type, Ray *ray, HitInfo *hit_info) {
    switch (type) {
        case SphereType:
            return sphere_intersection((Sphere *)object, ray, hit_info);
        case PlaneType:
            return plane_intersection((Plane *)object, ray);
        case TriangleType:
            return triangle_intersection((Triangle *)object, ray);
        case ObjectMeshType:
            return object_mesh_intersection((ObjectMesh *)object, ray, hit_info);
        default:
            return -1;
    }
}

Vec3 object_normal_at(Object *object, ObjectType type, Vec3 p) {
    switch (type) {
        case SphereType:
            return sphere_normal_at((Sphere *)object, p);
        case PlaneType:
            return plane_normal_at((Plane *)object);
        case TriangleType:
            return triangle_normal_at((Triangle *)object);
        default:
            return vec3_empty();
    }
}

Vec3 object_color(Object *object, ObjectType type) {
    switch (type) {
        case SphereType:
            return object->sphere.material->color;
        case PlaneType:
            return object->plane.material->color;
        case TriangleType:
            return object->triangle.material->color;
        case ObjectMeshType:
            Vec3 color = {1, 0, 0};
            return color;
        default:
            return vec3_empty();
    }
}

Material *object_material(Object *object, ObjectType type) {
    switch (type) {
        case SphereType:
            return object->sphere.material;
        case PlaneType:
            return object->plane.material;
        case TriangleType:
            return object->triangle.material;
        case ObjectMeshType:
            return object->object_mesh.material;
        default:
            return NULL;
    }
}

Vec3 ray_normal_at(Ray *ray, float t) { return vec3_add(ray->origin, vec3_mul(ray->direction, t)); }

Vec3 sphere_normal_at(Sphere *s, Vec3 p) { return vec3_mul(vec3_sub(p, s->position), 1.f / s->radius); }

Vec2 sphere_uv_texture_coord(Vec3 p) {
    float theta = acos(-p.y);
    float phi = atan2(-p.z, p.x) + M_PI;
    float u = phi / (2 * M_PI);
    float v = theta / M_PI;
    Vec2 uv = {u, v};
    return uv;
}

float sphere_intersection(Sphere *s, Ray *ray, HitInfo *hit_info) {
    Vec3 sphere_pos = {0, 0, 0};
    Vec3 oc = vec3_sub(ray->origin, s->position);
    float a = dot_product(ray->direction, ray->direction);
    float b = 2 * dot_product(oc, ray->direction);
    float c = dot_product(oc, oc) - s->radius * s->radius;
    float discriminant = b * b - 4.f * a * c;
    if (discriminant < 0) {
        return -1;
    } else {
        float t1 = (-b - sqrt(discriminant)) / (2 * a);
        float t2 = (-b + sqrt(discriminant)) / (2 * a);
        if (t1 > 1e-6) {
            return t1;
        } else if (t2 > 1e-6) {
            return t2;
        }
        return -1;
    }
}

Vec3 plane_normal_at(Plane *plane) { return plane->normal; }

float plane_intersection(Plane *plane, Ray *ray) {
    float denom = dot_product(ray->direction, plane->normal);
    if (fabs(denom) > 1e-6) {
        float nom = dot_product(vec3_sub(plane->position, ray->origin), plane->normal);
        float t = nom / denom;
        if (t >= 1e-6) return t;
    }
    return -1;
}

Vec3 triangle_normal_at(Triangle *t) {
    Vec3 A = vec3_sub(t->b, t->a);
    Vec3 B = vec3_sub(t->c, t->a);
    Vec3 Cnorm = cross_product(A, B);
    float mag = vec3_mag(Cnorm);
    Cnorm = vec3_mul(Cnorm, 1 / mag);
    float d = -dot_product(Cnorm, t->a);
    if (isnan(d) == 1) {
        fprintf(stderr, "Failed to load triangle");
        exit(1);
    }
    return Cnorm;
}

float triangle_intersection(Triangle *t, Ray *ray) {
    if (isnan(t->d) == 1) {
        triangle_normal_at(t);
    }
    float n_dot_ray_dir = dot_product(t->normal, ray->direction);
    if (fabs(n_dot_ray_dir) < 1e-4) {
        return -1;
    }
    float ti = -(t->d + dot_product(t->normal, ray->origin)) / n_dot_ray_dir;
    if (ti < 0) {
        return -1;
    }
    Vec3 Q = vec3_add(ray->origin, vec3_mul(ray->direction, ti));
    // printf("Q: (%f,%f,%f)\n", Q.x, Q.y, Q.z);
    Vec3 edge0 = vec3_sub(t->b, t->a);
    Vec3 vp0 = vec3_sub(Q, t->a);
    Vec3 C0 = cross_product(edge0, vp0);
    if (dot_product(t->normal, C0) < 0) {
        return -1;
    }
    Vec3 edge1 = vec3_sub(t->c, t->b);
    Vec3 vp1 = vec3_sub(Q, t->b);
    Vec3 C1 = cross_product(edge1, vp1);
    if (dot_product(t->normal, C1) < 0) {
        return -1;
    }
    Vec3 edge2 = vec3_sub(t->a, t->c);
    Vec3 vp2 = vec3_sub(Q, t->c);
    Vec3 C2 = cross_product(edge2, vp2);
    if (dot_product(t->normal, C2) < 0) {
        return -1;
    }
    return ti;
}

Vec3 light_direction(Object *object, ObjectType type, Vec3 point) {
    switch (type) {
        case DirectionalLightType:
            return object->directional_light.direction;
        case PointLightType:
            return vec3_neg(vec3_norm(vec3_sub(point, object->point_light.position)));
        default:
            printf("error with light_direction");
            abort();
    }
    return vec3_empty();
}

float light_intensity(Object *object, ObjectType type) {
    switch (type) {
        case DirectionalLightType:
            return object->directional_light.intensity;
        case PointLightType:
            return object->point_light.intensity;
        default:
            printf("error with light_intensity");
            abort();
    }
    return 0;
}

Sphere *sphere_create(Vec3 position, float radius, Material *material) {
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->position = position;
    sphere->radius = radius;
    sphere->material = material;
    return sphere;
}

Plane *plane_create(Vec3 position, Vec3 normal, Material *material) {
    Plane *plane = malloc(sizeof(Plane));
    plane->position = position;
    plane->normal = vec3_norm(normal);
    plane->material = material;
    return plane;
}

Triangle *triangle_create(Vec3 a, Vec3 b, Vec3 c, Material *material) {
    Triangle *triangle = malloc(sizeof(Triangle));
    triangle->a = a;
    triangle->b = b;
    triangle->c = c;
    triangle->material = material;
    triangle->normal = triangle_normal_at(triangle);
    triangle->d = -dot_product(triangle->normal, c);
    return triangle;
}

Triangle *triangle_create_with_normals(Vec3 a, Vec3 b, Vec3 c, Material *material, Vec3 normal1, Vec3 normal2,
                                       Vec3 normal3) {
    Triangle *triangle = malloc(sizeof(Triangle));
    triangle->a = a;
    triangle->b = b;
    triangle->c = c;
    triangle->material = material;
    triangle->normal1 = normal1;
    triangle->normal2 = normal2;
    triangle->normal3 = normal3;
    triangle->normal = triangle_normal_at(triangle);
    triangle->d = -dot_product(triangle->normal, c);
    printf("normal:%f,%f,%f\n", triangle->normal.x, triangle->normal.y, triangle->normal.z);
    // printf("d:%f\n", triangle->d);
    return triangle;
}

DirectionalLight *directional_light_create(Vec3 direction, Vec3 color, float intensity) {
    DirectionalLight *light = malloc(sizeof(DirectionalLight));
    light->direction = direction;
    light->color = color;
    light->intensity = intensity;
    return light;
}

PointLight *point_light_create(Vec3 position, Vec3 color, float intensity) {
    PointLight *light = malloc(sizeof(PointLight));
    light->position = position;
    light->color = color;
    light->intensity = intensity;
    return light;
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

void mesh_boundaries(Mesh *mesh, Vec3 *min_b, Vec3 *max_b, Vec3 *center_b) {
    // finds max and min boundaries
    // and create a sphere that fits that shape
    Vec3 min_bounds = {INFINITY, INFINITY, INFINITY};
    Vec3 max_bounds = {-INFINITY, -INFINITY, -INFINITY};
    Vec3 center = {0, 0, 0};
    for (int i = 0; i < mesh->vertex_count; i++) {
        float x = mesh->vertices[i].x;
        float y = mesh->vertices[i].y;
        float z = mesh->vertices[i].z;
        if (x < min_bounds.x) {
            min_bounds.x = x;
        }
        if (x > max_bounds.x) {
            max_bounds.x = x;
        }
        if (y < min_bounds.y) {
            min_bounds.y = y;
        }
        if (y > max_bounds.y) {
            max_bounds.y = y;
        }
        if (z < min_bounds.z) {
            min_bounds.z = z;
        }
        if (z > max_bounds.z) {
            max_bounds.z = z;
        }
    }
    center.x = sqrt(pow(min_bounds.x + max_bounds.x, 2)) / 2;
    center.y = sqrt(pow(min_bounds.y + max_bounds.y, 2)) / 2;
    center.z = sqrt(pow(min_bounds.z + max_bounds.z, 2)) / 2;

    printf("max_bounds: (%f, %f, %f)\n", max_bounds.x, max_bounds.y, max_bounds.z);
    printf("min_bounds: (%f, %f, %f)\n", min_bounds.x, min_bounds.y, min_bounds.z);
    printf("center: (%f, %f, %f)\n", center.x, center.y, center.z);

    *min_b = min_bounds;
    *max_b = max_bounds;
    *center_b = center;
}

ObjectMesh *object_mesh_create(Mesh *mesh, Material *material) {
    ObjectMesh *object_mesh = malloc(sizeof(ObjectMesh));
    Vec3 min_bounds, max_bounds, center;
    mesh_boundaries(mesh, &min_bounds, &max_bounds, &center);
    BoundingSphere *bounding_sphere = bounding_sphere_create(max_bounds, min_bounds, center);
    object_mesh->mesh = mesh;
    object_mesh->bounding_sphere = bounding_sphere;
    object_mesh->material = material;
    return object_mesh;
}

void mesh_set_face_normals(Mesh *mesh) {
    Vec3 *face_normals = malloc(sizeof(Vec3) * mesh->vertex_count / 3);
    float *directions = malloc(sizeof(float) * mesh->vertex_count / 3);
    int s = 0;
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n + s;
        int j = n + s + 1;
        int k = n + s + 2;
        Vec3 normal = mesh_triangle_normal_at(mesh->vertices[i], mesh->vertices[j], mesh->vertices[k]);
        float d = -dot_product(normal, mesh->vertices[k]);
        face_normals[n] = normal;
        directions[n] = d;
        s += 2;
    }
    mesh->face_normals = face_normals;
    mesh->directions = directions;
}

Vec3 mesh_triangle_normal_at(Vec3 a, Vec3 b, Vec3 c) {
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

float object_mesh_intersection(ObjectMesh *object_mesh, Ray *ray, HitInfo *hit_info) {
    Mesh *mesh = object_mesh->mesh;
    float closet_hit = INFINITY;
    int hit_index = 0;
    Vec3 barycentric;
    Vec3 closest_barycentric;
    // // check if in boundary sphere
    // float sphere_hit =
    // sphere_intersection(&object_mesh->bounding_sphere->sphere, ray); if
    // (sphere_hit < 0) {
    //     return -1;
    // }
    for (int n = 0; n < mesh->vertex_count / 3; n++) {
        int i = n * 3;
        int j = n * 3 + 1;
        int k = n * 3 + 2;
        float hit =
            mesh_triangle_intersection(ray, mesh->vertices[i], mesh->vertices[j], mesh->vertices[k], &barycentric);
        if (hit > 0 && hit < closet_hit) {
            closet_hit = hit;
            hit_index = n;
            closest_barycentric = barycentric;
        }
    }
    if (closet_hit != INFINITY) {
        Vec3 hit_normal = vec3_add(vec3_add(vec3_mul(mesh->vertex_normals[hit_index * 3], closest_barycentric.x),
                                            vec3_mul(mesh->vertex_normals[hit_index * 3 + 1], closest_barycentric.y)),
                                   vec3_mul(mesh->vertex_normals[hit_index * 3 + 2], closest_barycentric.z));
        // hit_normal = vec3_norm(hit_normal);
        // Vec3 hit_normal =  mesh->face_normals[hit_index];
        // hit_info->color = vec3_component_mul(closest_barycentric, (Vec3)
        // { 1.f, 1.f, 1.f });
        hit_info->position = ray_normal_at(ray, closet_hit);
        hit_info->hit = closet_hit;
        bool front_face = dot_product(vec3_neg(ray->direction), hit_normal) < 0.0;
        hit_info->front_face = front_face;
        hit_info->normal = front_face ? hit_normal : vec3_neg(hit_normal);
        hit_info->material = object_mesh->material;
        float u = (mesh->texture_uv[hit_index * 3].x * closest_barycentric.x +
                   mesh->texture_uv[hit_index * 3 + 1].x * closest_barycentric.x +
                   mesh->texture_uv[hit_index * 3 + 2].x * closest_barycentric.x) /
                  3.f;
        float v = (mesh->texture_uv[hit_index * 3].y * closest_barycentric.y +
                   mesh->texture_uv[hit_index * 3 + 1].y * closest_barycentric.y +
                   mesh->texture_uv[hit_index * 3 + 2].y * closest_barycentric.y) /
                  3.f;
        hit_info->u = u;
        hit_info->v = v;
        int i = u * object_mesh->material->texture->width;
        int j = v * object_mesh->material->texture->height;
        // hit_info->color = texture_pixel_data(object_mesh->material->texture, i,
        // j);
        hit_info->color = texture_checkboard(u, v, 0.08f);
    }
    return closet_hit;
}

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction) {
    Material *material = malloc(sizeof(Material));
    material->albedo = albedo;
    material->Kd = Kd;
    material->Ks = Ks;
    material->reflective = reflective;
    material->color = color;
    material->index_of_refraction = index_of_refraction;
    return material;
}
