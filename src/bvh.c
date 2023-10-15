#include <bvh.h>

static void calculate_bounding_box(TriangleBVH *t, AABoundingBox *box) {
    float max_x = fmax(fmax(t->v1.x, t->v2.x), t->v3.x) + EPSILON;
    float max_y = fmax(fmax(t->v1.y, t->v2.y), t->v3.y) + EPSILON;
    float max_z = fmax(fmax(t->v1.z, t->v2.z), t->v3.z) + EPSILON;

    float min_x = fmin(fmin(t->v1.x, t->v2.x), t->v3.x) - EPSILON;
    float min_y = fmin(fmin(t->v1.y, t->v2.y), t->v3.y) - EPSILON;
    float min_z = fmin(fmin(t->v1.z, t->v2.z), t->v3.z) - EPSILON;

    box->max = (Vec3){max_x, max_y, max_z};
    box->min = (Vec3){min_x, min_y, min_z};
}

static void calculate_centroid(TriangleBVH *t, Vec3 *centroid) {
    centroid->x = (t->v1.x + t->v2.x + t->v3.x) / 3.f;
    centroid->y = (t->v1.y + t->v2.y + t->v3.y) / 3.f;
    centroid->z = (t->v1.z + t->v2.z + t->v3.z) / 3.f;
}

static void calculate_centroid_from_box(AABoundingBox *box, Vec3 *centroid) {
    Vec3 dist = vec3_mul(vec3_distance(box->max, box->min), 0.5f);
    *centroid = vec3_add(box->min, dist);
}

static void calculate_bounding_box_range(Primatives *primatives, int lo, int hi, AABoundingBox *box_range) {
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

static void calculate_centroid_range(Primatives *primatives, int lo, int hi, Vec3 *centroid) {
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
    *centroid = (Vec3){dist_x, dist_y, dist_z};
}

static int argmax(float *array, size_t size) {
    float max_ = -INFINITY;
    int max_index = 0;
    for (int i = 0; i < size; i++) {
        if (array[i] > max_ + EPSILON) {
            max_ = array[i];
            max_index = i;
        }
    }
    return max_index;
}

static void bvh_swap_primatives(Primatives *primatives, int a, int b) {
    PrimativeInfo temp = primatives->array[a];
    primatives->array[a] = primatives->array[b];
    primatives->array[b] = temp;
}

static int bvh_partition(Primatives *primatives, int lo, int hi, int axis, float pivot) {
    for (int i = lo; i < hi; i++) {
        float point = vec3_index_value(&primatives->array[i].centroid, axis);
        if (point <= pivot) {
            bvh_swap_primatives(primatives, lo, i);
            lo += 1;
        }
    }
    return lo;
}

Primatives *bvh_prepare_data(TriangleBVH *triangles, size_t size) {
    Primatives *primatives = malloc(sizeof(Primatives));
    primatives->array = malloc(sizeof(PrimativeInfo) * size);
    primatives->size = size;
    for (int i = 0; i < size; i++) {
        calculate_bounding_box(&triangles[i], &primatives->array[i].aabb);
        calculate_centroid(&triangles[i], &primatives->array[i].centroid);
        primatives->array[i].index = i;
    }
    return primatives;
}

BVHNode *bvh_build_child(Primatives *primatives, int lo, int hi, int depth) {
    BVHNode *node = malloc(sizeof(BVHNode));
    int range_size = hi - lo;
    if (depth > BVH_MAX_DEPTH) {
        printf("MAX DEPTH REACHED\n");
        AABoundingBox bb_range;
        calculate_bounding_box_range(primatives, lo, hi, &bb_range);
        int index = primatives->array[lo].index;
        *node = (BVHNode){&bb_range, index, true, NULL, NULL};
    } else if (range_size == 0) {
        node = NULL;
    } else if (range_size == 1) {
        *node = (BVHNode){&primatives->array[lo].aabb, primatives->array[lo].index, true, NULL, NULL};
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
        calculate_centroid_range(primatives, lo, hi, &centroid_range);
        float array_index[3] = {centroid_range.x, centroid_range.y, centroid_range.z};
        // choose biggest centroid range dimension
        int axis = argmax(array_index, 3);
        // find median on axis
        float offset = vec3_index_value(&centroid_range, axis) / 2.f;
        float median = vec3_index_value(&bb_range->min, axis) + offset;
        int pivot = bvh_partition(primatives, lo, hi, axis, median);
        if (pivot == lo) {  // flip split plane to other side if no partition is found
            median = vec3_index_value(&bb_range->max, axis) - offset;
            pivot = bvh_partition(primatives, lo, hi, axis, median);
        }
        if (pivot == hi) {
            pivot = hi - ((hi - lo) / 2);
            bvh_swap_primatives(primatives, lo, pivot);
        }
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
        // printf("box(max=(%f,%f,%f), min=(%f,%f,%f)),", node->aabb->max.x, node->aabb->max.y, node->aabb->max.z,
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
}