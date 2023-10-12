#include <bvh.h>

void calculate_bounding_box(TriangleBVH *t, AABoundingBox *box) {
    float max_x = fmax(fmax(t->v1.x, t->v2.x), t->v3.x) + EPSILON;
    float max_y = fmax(fmax(t->v1.y, t->v2.y), t->v3.y) + EPSILON;
    float max_z = fmax(fmax(t->v1.z, t->v2.z), t->v3.z) + EPSILON;

    float min_x = fmin(fmin(t->v1.x, t->v2.x), t->v3.x) - EPSILON;
    float min_y = fmin(fmin(t->v1.y, t->v2.y), t->v3.y) - EPSILON;
    float min_z = fmin(fmin(t->v1.z, t->v2.z), t->v3.z) - EPSILON;

    box->max = (Vec3){max_x, max_y, max_z};
    box->min = (Vec3){min_x, min_y, min_z};
}

void calculate_centroid(TriangleBVH *t, Vec3 *centroid) {
    centroid->x = (t->v1.x + t->v2.x + t->v3.x) / 3.f;
    centroid->y = (t->v1.y + t->v2.y + t->v3.y) / 3.f;
    centroid->z = (t->v1.z + t->v2.z + t->v3.z) / 3.f;
}

void calculate_centroid_from_box(AABoundingBox *box, Vec3 *centroid) {
    Vec3 dist = vec3_mul(vec3_distance(box->max, box->min), 0.5f);
    *centroid = vec3_add(box->min, dist);
}

void calculate_bounding_box_range(AABoundingBox *boxes, size_t size, AABoundingBox *box_range) {
    Vec3 max_ = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    Vec3 min_ = (Vec3){INFINITY, INFINITY, INFINITY};
    for (int i = 0; i < size; i++) {
        max_.x = fmax(max_.x, boxes[i].max.x);
        min_.x = fmin(min_.x, boxes[i].min.x);
        max_.y = fmax(max_.y, boxes[i].max.y);
        min_.y = fmin(min_.y, boxes[i].min.y);
        max_.z = fmax(max_.z, boxes[i].max.z);
        min_.z = fmin(min_.z, boxes[i].min.z);
    }
    box_range->max = max_;
    box_range->min = min_;
}

void calculate_centroid_range(Vec3 *centroids, size_t size, Vec3 *centroid) {
    Vec3 max_ = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    Vec3 min_ = (Vec3){INFINITY, INFINITY, INFINITY};
    for (int i = 0; i < size; i++) {
        max_.x = fmax(max_.x, centroids[i].x);
        min_.x = fmin(min_.x, centroids[i].x);
        max_.y = fmax(max_.y, centroids[i].y);
        min_.y = fmin(min_.y, centroids[i].y);
        max_.z = fmax(max_.z, centroids[i].z);
        min_.z = fmin(min_.z, centroids[i].z);
    }
    float dist_x = fabs(max_.x - min_.x);
    float dist_y = fabs(max_.y - min_.y);
    float dist_z = fabs(max_.z - min_.z);
    *centroid = (Vec3){dist_x, dist_y, dist_z};
}

int argmax(int *array, size_t size) {
    float max_ = -INFINITY;
    int max_index = 0;
    for (int i = 0; i < size; i++) {
        if (array[i] > max_) {
            max_ = array[i];
            max_index = i;
        }
    }
    return max_index;
}

BVHPreparedData *bvh_prepare_data(TriangleBVH *triangles, size_t size) {
    BVHPreparedData *data = malloc(sizeof(BVHPreparedData));
    data->boxes = malloc(sizeof(AABoundingBox) * size);
    data->centroids = malloc(sizeof(Vec3) * size);
    data->indexes = malloc(sizeof(int) * size);
    data->size = size;
    for (int i = 0; i < size; i++) {
        calculate_bounding_box(&triangles[i], &data->boxes[i]);
        calculate_centroid(&triangles[i], &data->centroids[i]);
        data->indexes[i] = i;
    }
    return data;
}

void bvh_build_tree(BVHPreparedData *p_data, BVHNode *node, int depth) {
    if (depth > 8) {
        printf("MAX DEPTH REACHED\n");
    }
    if (p_data->size == 0) {
        return;
    }

    if (node == NULL) {
        node = malloc(sizeof(BVHNode));
    }
    AABoundingBox bb_range;
    calculate_bounding_box_range(p_data->boxes, p_data->size, &bb_range);
    node->aabb = &bb_range;

    Vec3 centroid_range;
    calculate_centroid_range(p_data->centroids, p_data->size, &centroid_range);

    int array_index[3] = {centroid_range.x, centroid_range.y, centroid_range.z};
    int axis_index = argmax(array_index, 3);
    // find median on axis
    float offset = vec3_index_value(&centroid_range, axis_index) / 2.f;
    float median_point_pos = vec3_index_value(&bb_range.min, axis_index) + offset;
}
