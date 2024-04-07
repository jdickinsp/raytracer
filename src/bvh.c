#include <bvh.h>

/**
 * @brief Finds the index of the maximum absolute value in an array.
 *
 * @param array The input array.
 * @param size The size of the array.
 * @return The index of the maximum absolute value.
 */
static size_t argmax(const float *array, size_t size) {
    if (size == 0) {
        return 0;  // Return 0 if the array is empty
    }
    size_t max_index = 0;
    float max_val = array[0];
    for (size_t i = 1; i < size; i++) {
        if (array[i] > max_val) {
            max_val = array[i];
            max_index = i;
        }
    }
    return max_index;
}

/**
 * @brief Calculates the bounding box of a triangle.
 *
 * @param t The input triangle.
 * @param box The output bounding box.
 */
static void calculate_bounding_box(const BVTriangle *t, AABoundingBox *box) {
    box->max.x = fmaxf(fmaxf(t->v1.x, t->v2.x), t->v3.x);
    box->max.y = fmaxf(fmaxf(t->v1.y, t->v2.y), t->v3.y);
    box->max.z = fmaxf(fmaxf(t->v1.z, t->v2.z), t->v3.z);

    box->min.x = fminf(fminf(t->v1.x, t->v2.x), t->v3.x) - EPSILON;
    box->min.y = fminf(fminf(t->v1.y, t->v2.y), t->v3.y) - EPSILON;
    box->min.z = fminf(fminf(t->v1.z, t->v2.z), t->v3.z) - EPSILON;
}

/**
 * @brief Calculates the centroid of a triangle.
 *
 * @param t The input triangle.
 * @param centroid The output centroid.
 */
static void calculate_centroid(const BVTriangle *t, Vec3 *centroid) {
    centroid->x = (t->v1.x + t->v2.x + t->v3.x) / 3.0f;
    centroid->y = (t->v1.y + t->v2.y + t->v3.y) / 3.0f;
    centroid->z = (t->v1.z + t->v2.z + t->v3.z) / 3.0f;
}

/**
 * @brief Calculates the centroid of a bounding box.
 *
 * @param box The input bounding box.
 * @param centroid The output centroid.
 */
static void calculate_centroid_from_box(const AABoundingBox *box, Vec3 *centroid) {
    Vec3 diagonal = vec3_sub(box->max, box->min);
    *centroid = vec3_add(box->min, vec3_mul(diagonal, 0.5f));
}

/**
 * @brief Calculates the bounding box of a range of primitives.
 *
 * @param primitives The array of primitives.
 * @param lo The start index of the range.
 * @param hi The end index of the range (exclusive).
 * @param box_range The output bounding box of the range.
 */
static void calculate_bounding_box_range(const Primitives *primitives, size_t lo, size_t hi, AABoundingBox *box_range) {
    Vec3 min_point = (Vec3){INFINITY, INFINITY, INFINITY};
    Vec3 max_point = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    for (size_t i = lo; i < hi; i++) {
        const AABoundingBox *box = &primitives->array[i].aabb;
        min_point.x = fminf(min_point.x, box->min.x);
        min_point.y = fminf(min_point.y, box->min.y);
        min_point.z = fminf(min_point.z, box->min.z);
        max_point.x = fmaxf(max_point.x, box->max.x);
        max_point.y = fmaxf(max_point.y, box->max.y);
        max_point.z = fmaxf(max_point.z, box->max.z);
    }
    box_range->min = min_point;
    box_range->max = max_point;
}

/**
 * @brief Calculates the centroid range of a range of primitives.
 *
 * @param primitives The array of primitives.
 * @param lo The start index of the range.
 * @param hi The end index of the range (exclusive).
 * @param centroid_range The output centroid range.
 * @param centroid_min The output minimum centroid.
 */
static void calculate_centroid_range(const Primitives *primitives, size_t lo, size_t hi, Vec3 *centroid_range,
                                     Vec3 *centroid_min) {
    Vec3 min_point = (Vec3){INFINITY, INFINITY, INFINITY};
    Vec3 max_point = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    for (size_t i = lo; i < hi; i++) {
        const Vec3 *centroid = &primitives->array[i].centroid;
        min_point.x = fminf(min_point.x, centroid->x);
        min_point.y = fminf(min_point.y, centroid->y);
        min_point.z = fminf(min_point.z, centroid->z);
        max_point.x = fmaxf(max_point.x, centroid->x);
        max_point.y = fmaxf(max_point.y, centroid->y);
        max_point.z = fmaxf(max_point.z, centroid->z);
    }
    *centroid_range = vec3_sub(max_point, min_point);
    *centroid_min = min_point;
}

/**
 * @brief Swaps two primitives in the primitives array.
 *
 * @param primitives The array of primitives.
 * @param a The index of the first primitive.
 * @param b The index of the second primitive.
 */
static void bvh_swap_primitives(Primitives *primitives, size_t a, size_t b) {
    PrimitiveInfo temp = primitives->array[a];
    primitives->array[a] = primitives->array[b];
    primitives->array[b] = temp;
}

/**
 * @brief Partitions the primitives array based on a pivot value.
 *
 * @param primitives The array of primitives.
 * @param lo The start index of the range to partition.
 * @param hi The end index of the range to partition (exclusive).
 * @param axis The axis to partition on (0 = x, 1 = y, 2 = z).
 * @param pivot The pivot value.
 * @return The index of the first element greater than the pivot.
 */
static size_t bvh_partition(Primitives *primitives, size_t lo, size_t hi, int axis, float pivot) {
    size_t i = lo;
    for (size_t j = lo; j < hi; j++) {
        float coord = vec3_index_value(&primitives->array[j].centroid, axis);
        if (coord <= pivot) {
            bvh_swap_primitives(primitives, i, j);
            i++;
        }
    }
    return i;
}

/**
 * @brief Prepares the primitives data for BVH construction.
 *
 * @param triangles The array of triangles.
 * @param size The number of triangles.
 * @return The prepared primitives data.
 */
Primitives *bvh_prepare_data(const BVTriangle *triangles, size_t size) {
    Primitives *primitives = malloc(sizeof(Primitives));
    if (primitives == NULL) {
        return NULL;
    }

    primitives->array = malloc(sizeof(PrimitiveInfo) * size);
    primitives->triangles = malloc(sizeof(BVTriangle) * size);
    if (primitives->array == NULL || primitives->triangles == NULL) {
        free(primitives->array);
        free(primitives->triangles);
        free(primitives);
        return NULL;
    }

    primitives->size = size;
    for (size_t i = 0; i < size; i++) {
        calculate_bounding_box(&triangles[i], &primitives->array[i].aabb);
        calculate_centroid(&triangles[i], &primitives->array[i].centroid);
        primitives->array[i].tri_index = (int)i;
        primitives->triangles[i] = triangles[i];
    }

    return primitives;
}

/**
 * @brief Builds a child node of the BVH tree recursively.
 *
 * @param primitives The array of primitives.
 * @param lo The start index of the primitives range.
 * @param hi The end index of the primitives range (exclusive).
 * @param depth The current depth of the node in the BVH tree.
 * @return The created child node.
 */
BVHNode *bvh_build_child(const Primitives *primitives, size_t lo, size_t hi, int depth) {
    if (lo >= hi) {
        return NULL;  // Return NULL if the range is empty or invalid
    }

    BVHNode *node = malloc(sizeof(BVHNode));
    if (node == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    int range_size = hi - lo;
    if (depth > BVH_MAX_DEPTH) {
        // printf("MAX DEPTH REACHED (%i,%i)\n", lo, hi);
        AABoundingBox bb_range;
        calculate_bounding_box_range(primitives, lo, hi, &bb_range);
        *node = (BVHNode){&bb_range, lo, true, NULL, NULL};
    } else if (range_size == 0) {
        node = NULL;
    } else if (range_size == 1) {
        int index = primitives->array[lo].tri_index;
        *node = (BVHNode){&primitives->array[lo].aabb, index, true, NULL, NULL};
    } else {
        *node = (BVHNode){NULL, -1, false, NULL, NULL};
    }
    return node;
}

/**
 * @brief Builds the BVH tree recursively.
 *
 * @param primitives The array of primitives.
 * @return The root node of the BVH tree.
 */
BVHNode *bvh_build_tree(Primitives *primitives) {
    int depth = 0;
    int lo = 0;
    int hi = primitives->size;
    Queue *visit = queue_init();
    BVHNode *root = bvh_build_child(primitives, lo, hi, depth);
    Tuple3Item item = {root, sizeof(BVHNode), lo, hi, depth};
    BVHNode *node;
    queue_add(visit, &item, sizeof(Tuple3Item));
    while (visit->size > 0) {
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
        calculate_bounding_box_range(primitives, lo, hi, bb_range);
        node->aabb = bb_range;
        Vec3 centroid_range;
        Vec3 centroid_min;
        float scale_factor = 0.5f;
        calculate_centroid_range(primitives, lo, hi, &centroid_range, &centroid_min);
        // choose biggest centroid range dimension
        int axis = vec3_argmax(&centroid_range);
        // find median on axis
        float offset = vec3_index_value(&centroid_range, axis) * scale_factor;
        float median = vec3_index_value(&centroid_min, axis) + offset + EPSILON;
        int pivot = bvh_partition(primitives, lo, hi, axis, median);
        if (pivot - lo > 0) {
            node->left = bvh_build_child(primitives, lo, pivot, depth + 1);
            Tuple3Item *item_left = tuple3_item_create(node->left, sizeof(BVHNode), lo, pivot, depth + 1);
            queue_add(visit, item_left, sizeof(Tuple3Item));
        }
        if (hi - pivot > 0) {
            node->right = bvh_build_child(primitives, pivot, hi, depth + 1);
            Tuple3Item *item_right = tuple3_item_create(node->right, sizeof(BVHNode), pivot, hi, depth + 1);
            queue_add(visit, item_right, sizeof(Tuple3Item));
        }
    }
    queue_free(visit);
    return root;
}

/**
 * @brief Pretty-prints the BVH tree.
 *
 * @param root The root node of the BVH tree.
 */
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
    while (visit->size > 0) {
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

/**
 * @brief Traverses the BVH tree and prints the leaf node data.
 *
 * @param root The root node of the BVH tree.
 */
void bvh_traverse_tree(BVHNode *root) {
    Queue *visit = queue_init();
    if (visit == NULL) {
        return;
    }

    queue_add(visit, root, sizeof(BVHNode));

    while (visit->size > 0) {
        BVHNode node;
        queue_pop(visit, &node);

        if (node.is_leaf) {
            printf("%d, ", node.data);
        } else {
            queue_add(visit, node.right, sizeof(BVHNode));
            queue_add(visit, node.left, sizeof(BVHNode));
        }
    }

    printf("\n");
    queue_free(visit);
}

/**
 * @brief Finds the ray from a triangle's centroid to the origin.
 *
 * @param origin The origin point.
 * @param triangle The triangle.
 * @param ray The output ray.
 */
void find_ray_from_triangle(const Vec3 origin, const BVTriangle *triangle, Ray *ray) {
    Vec3 centroid;
    calculate_centroid(triangle, &centroid);
    ray->origin = origin;
    ray->direction = vec3_norm(vec3_sub(centroid, origin));
    ray->t = 0.0f;
}

/**
 * @brief Calculates the inverse of the ray direction component.
 *
 * @param v The ray direction component.
 * @return The inverse of the ray direction component.
 */
static inline float inv_ray_direction(float v) { return 1.0f / v; }

/**
 * @brief Tests if a ray intersects with a bounding box.
 *
 * @param box The bounding box.
 * @param ray The ray.
 * @param t The output intersection distance.
 * @return True if the ray intersects the bounding box, false otherwise.
 */
bool bounding_box_intersection(const AABoundingBox *box, const Ray *ray, float *t) {
    float inv_dir_x = inv_ray_direction(ray->direction.x);
    float inv_dir_y = inv_ray_direction(ray->direction.y);
    float inv_dir_z = inv_ray_direction(ray->direction.z);

    float tx1 = (box->min.x - ray->origin.x) * inv_dir_x;
    float tx2 = (box->max.x - ray->origin.x) * inv_dir_x;
    float ty1 = (box->min.y - ray->origin.y) * inv_dir_y;
    float ty2 = (box->max.y - ray->origin.y) * inv_dir_y;
    float tz1 = (box->min.z - ray->origin.z) * inv_dir_z;
    float tz2 = (box->max.z - ray->origin.z) * inv_dir_z;

    float tmin = fmaxf(fmaxf(fminf(tx1, tx2), fminf(ty1, ty2)), fminf(tz1, tz2));
    float tmax = fminf(fminf(fmaxf(tx1, tx2), fmaxf(ty1, ty2)), fmaxf(tz1, tz2));

    if (tmax < 0.0f || tmin > tmax) {
        *t = tmax;
        return false;
    }

    *t = tmin;
    return true;
}
