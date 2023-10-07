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

// void build_bvh_tree(AABoundingBox *boxes, Vec3 *centroids, int *indexes, int depth) {
//     //
// }