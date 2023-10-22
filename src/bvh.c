#include <bvh.h>

static int argmax(const float *array, size_t size) {
    float max_ = -INFINITY;
    int max_index = 0;
    for (int i = 0; i < size; i++) {
        if (fabs(array[i]) > max_) {
            max_ = fabs(array[i]);
            max_index = i;
        }
    }
    return max_index;
}

static void calculate_bounding_box(const BVTriangle *t, AABoundingBox *box) {
    float max_x = fmax(fmax(t->v1.x, t->v2.x), t->v3.x);
    float max_y = fmax(fmax(t->v1.y, t->v2.y), t->v3.y);
    float max_z = fmax(fmax(t->v1.z, t->v2.z), t->v3.z);

    float min_x = fmin(fmin(t->v1.x, t->v2.x), t->v3.x);
    float min_y = fmin(fmin(t->v1.y, t->v2.y), t->v3.y);
    float min_z = fmin(fmin(t->v1.z, t->v2.z), t->v3.z);

    box->max = (Vec3){max_x, max_y, max_z};
    box->min = (Vec3){min_x - EPSILON, min_y - EPSILON, min_z - EPSILON};
}

static void calculate_centroid(const BVTriangle *t, Vec3 *centroid) {
    centroid->x = (t->v1.x + t->v2.x + t->v3.x) / 3.f;
    centroid->y = (t->v1.y + t->v2.y + t->v3.y) / 3.f;
    centroid->z = (t->v1.z + t->v2.z + t->v3.z) / 3.f;
}

static void calculate_centroid_from_box(const AABoundingBox *box, Vec3 *centroid) {
    Vec3 dist = vec3_mul(vec3_distance(box->max, box->min), 0.5f);
    *centroid = vec3_add(box->min, dist);
}

static void calculate_bounding_box_range(const Primatives *primatives, int lo, int hi, AABoundingBox *box_range) {
    Vec3 max_ = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    Vec3 min_ = (Vec3){INFINITY, INFINITY, INFINITY};
    for (int i = lo; i < hi; i++) {
        AABoundingBox box = primatives->array[i].aabb;
        max_.x = fmax(max_.x, box.max.x);
        min_.x = fmin(min_.x, box.min.x);
        max_.y = fmax(max_.y, box.max.y);
        min_.y = fmin(min_.y, box.min.y);
        max_.z = fmax(max_.z, box.max.z);
        min_.z = fmin(min_.z, box.min.z);
    }
    box_range->max = max_;
    box_range->min = min_;
}

static void calculate_centroid_range(const Primatives *primatives, int lo, int hi, Vec3 *centroid_range,
                                     Vec3 *centroid_min) {
    Vec3 max_ = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    Vec3 min_ = (Vec3){INFINITY, INFINITY, INFINITY};
    for (int i = lo; i < hi; i++) {
        Vec3 c = primatives->array[i].centroid;
        max_.x = fmax(max_.x, c.x);
        min_.x = fmin(min_.x, c.x);
        max_.y = fmax(max_.y, c.y);
        min_.y = fmin(min_.y, c.y);
        max_.z = fmax(max_.z, c.z);
        min_.z = fmin(min_.z, c.z);
    }
    float dist_x = fabs(max_.x - min_.x);
    float dist_y = fabs(max_.y - min_.y);
    float dist_z = fabs(max_.z - min_.z);
    *centroid_range = (Vec3){dist_x, dist_y, dist_z};
    *centroid_min = min_;
}

static void bvh_swap_primatives(Primatives *primatives, int a, int b) {
    PrimativeInfo temp = primatives->array[a];
    primatives->array[a] = primatives->array[b];
    primatives->array[b] = temp;
}

static int bvh_partition(Primatives *primatives, int lo, int hi, int axis, float pivot, bool swap, int depth) {
    int nlo = lo;
    for (int i = lo; i < hi; i++) {
        float point = vec3_index_value(&primatives->array[i].centroid, axis);
        // if (depth >= BVH_MAX_DEPTH) {
        //     printf("\t(%f <= %f)\n", point, pivot);
        // }
        if (point <= pivot) {
            bvh_swap_primatives(primatives, lo, i);
            nlo += 1;
        }
    }
    return nlo;
}

Primatives *bvh_prepare_data(const BVTriangle *triangles, size_t size) {
    Primatives *primatives = malloc(sizeof(Primatives));
    primatives->array = malloc(sizeof(PrimativeInfo) * size);
    primatives->triangles = malloc(sizeof(BVTriangle) * size);
    primatives->size = size;
    for (int i = 0; i < size; i++) {
        calculate_bounding_box(&triangles[i], &primatives->array[i].aabb);
        calculate_centroid(&triangles[i], &primatives->array[i].centroid);
        primatives->array[i].tri_index = i;
        primatives->triangles[i] = triangles[i];
    }
    return primatives;
}

BVHNode *bvh_build_child(const Primatives *primatives, int lo, int hi, int depth) {
    BVHNode *node = malloc(sizeof(BVHNode));
    int range_size = hi - lo;
    if (depth > BVH_MAX_DEPTH) {
        printf("MAX DEPTH REACHED (%i,%i)\n", lo, hi);
        AABoundingBox bb_range;
        calculate_bounding_box_range(primatives, lo, hi, &bb_range);
        *node = (BVHNode){&bb_range, lo, true, NULL, NULL};
    } else if (range_size == 0) {
        node = NULL;
    } else if (range_size == 1) {
        int index = primatives->array[lo].tri_index;
        *node = (BVHNode){&primatives->array[lo].aabb, index, true, NULL, NULL};
    } else {
        *node = (BVHNode){NULL, -1, false, NULL, NULL};
    }
    return node;
};

BVHNode *bvh_build_tree(Primatives *primatives) {
    int depth = 0;
    int lo = 0;
    int hi = primatives->size;
    Queue *visit = queue_init();
    BVHNode *root = bvh_build_child(primatives, lo, hi, depth);
    Tuple3Item item = {root, sizeof(BVHNode), lo, hi, depth};
    BVHNode *node;
    queue_add(visit, &item, sizeof(Tuple3Item));
    while (visit->count > 0) {
        queue_pop(visit, &item);
        node = item.data;
        lo = item.x;
        hi = item.y;
        depth = item.z;
        if (node->is_leaf == true || node == NULL) {
            continue;
        }
        // calculate subdivided bounding boxes
        AABoundingBox *bb_range = malloc(sizeof(AABoundingBox));
        calculate_bounding_box_range(primatives, lo, hi, bb_range);
        node->aabb = bb_range;
        Vec3 centroid_range;
        Vec3 centroid_min;
        float scale_factor = 0.5f;
        calculate_centroid_range(primatives, lo, hi, &centroid_range, &centroid_min);
        // choose biggest centroid range dimension
        int axis = vec3_argmax(&centroid_range);
        // find median on axis
        float offset = vec3_index_value(&centroid_range, axis) * scale_factor;
        float median = vec3_index_value(&centroid_min, axis) + offset + EPSILON;
        int pivot = bvh_partition(primatives, lo, hi, axis, median, true, depth);
        if (pivot - lo > 0) {
            node->left = bvh_build_child(primatives, lo, pivot, depth + 1);
            Tuple3Item *item_left = tuple3_item_create(node->left, sizeof(BVHNode), lo, pivot, depth + 1);
            queue_add(visit, item_left, sizeof(Tuple3Item));
        }
        if (hi - pivot > 0) {
            node->right = bvh_build_child(primatives, pivot, hi, depth + 1);
            Tuple3Item *item_right = tuple3_item_create(node->right, sizeof(BVHNode), pivot, hi, depth + 1);
            queue_add(visit, item_right, sizeof(Tuple3Item));
        }
    }
    queue_free(visit);
    return root;
}

void bvh_pprint(BVHNode *root) {
    int indent = 0;
    int parens = 0;
    int depth = 0;
    bool is_left = false;
    Queue *visit = queue_init();
    Tuple3Item *item = tuple3_item_create(root, sizeof(BVHNode), indent, parens, 0);
    Tuple3Item current;
    BVHNode *node;
    queue_add(visit, item, sizeof(Tuple3Item));
    while (visit->count > 0) {
        queue_pop(visit, &current);
        node = current.data;
        indent = current.x;
        parens = current.y;
        depth = current.z;
        is_left = parens == 0;
        int space = indent * 2;
        printf("%*s", space, "");
        if (depth == 0) {
            printf("h:");
        } else if (is_left) {
            printf("l:");
        } else if (!is_left) {
            printf("r:");
        }
        // printf("box(max=(%f,%f,%f), min=(%f,%f,%f)),", node->aabb->max.x, node->aabb->max.y,
        // node->aabb->max.z,
        //        node->aabb->min.x, node->aabb->min.y, node->aabb->min.z);
        if (node->is_leaf == true) {
            printf("=>(d=%i", node->data);
            printf("%.*s,\n", is_left ? 1 : parens, ")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))");
        } else {
            printf("->(,\n");
            Tuple3Item *item_r = tuple3_item_create(node->right, sizeof(BVHNode), indent + 1, parens + 1, depth + 1);
            queue_add(visit, item_r, sizeof(Tuple3Item));
            Tuple3Item *item_l = tuple3_item_create(node->left, sizeof(BVHNode), indent + 1, 0, depth + 1);
            queue_add(visit, item_l, sizeof(Tuple3Item));
        }
    }
    queue_free(visit);
}

void bvh_traverse_tree(BVHNode *root) {
    Queue *visit = queue_init();
    queue_add(visit, root, sizeof(BVHNode));
    BVHNode node;
    while (visit->count) {
        queue_pop(visit, &node);
        if (node.is_leaf == true) {
            printf("%i, ", node.data);
        } else {
            queue_add(visit, node.right, sizeof(BVHNode));
            queue_add(visit, node.left, sizeof(BVHNode));
        }
    }
    printf("\n");
    queue_free(visit);
}

void find_ray_from_triangle(const Vec3 origin, const BVTriangle *triangle, Ray *ray) {
    Vec3 centroid;
    calculate_centroid(triangle, &centroid);
    Vec3 unit_dir = vec3_norm(vec3_sub(centroid, origin));
    *ray = (Ray){origin, unit_dir, 0};
}

float inv_ray_direction(float v) { return 1.0f / v; }

bool bounding_box_intersection(const AABoundingBox *box, Ray *ray, float *t) {
    float inv_ray_dir_x = inv_ray_direction(ray->direction.x);
    float inv_ray_dir_y = inv_ray_direction(ray->direction.y);
    float inv_ray_dir_z = inv_ray_direction(ray->direction.z);
    float tx1 = (box->min.x - ray->origin.x) * inv_ray_dir_x;
    float tx2 = (box->max.x - ray->origin.x) * inv_ray_dir_x;
    float ty1 = (box->min.y - ray->origin.y) * inv_ray_dir_y;
    float ty2 = (box->max.y - ray->origin.y) * inv_ray_dir_y;
    float tz1 = (box->min.z - ray->origin.z) * inv_ray_dir_z;
    float tz2 = (box->max.z - ray->origin.z) * inv_ray_dir_z;
    float tmin = fmax(fmax(fmin(tx1, tx2), fmin(ty1, ty2)), fmin(tz1, tz2));
    float tmax = fmin(fmin(fmax(tx1, tx2), fmax(ty1, ty2)), fmax(tz1, tz2));
    if (tmax < 0 || tmin > tmax) {
        *t = tmax;
        return false;
    }
    *t = tmin;
    return true;
}
