#ifndef BVH_H
#define BVH_H

#include <math.h>
#include <vectors.h>

static float EPSILON = 1e-4;

typedef struct {
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
} TriangleBVH;

typedef struct AABoundingBox {
    Vec3 min;
    Vec3 max;
} AABoundingBox;

typedef struct BVHLeaf {
    int data;
    AABoundingBox *aabb;
} BVHLeft;

typedef struct BVHNode {
    AABoundingBox *aabb;
    struct BVHNode *left;
    struct BVHNode *right;
} BVHNode;

typedef struct {
    AABoundingBox *boxes;
    Vec3 *centroids;
    int *indexes;
    size_t size;
} BVHPreparedData;

void calculate_bounding_box(TriangleBVH *triangle, AABoundingBox *box);
void calculate_centroid(TriangleBVH *t, Vec3 *centroid);
void calculate_centroid_from_box(AABoundingBox *box, Vec3 *centroid);
void calculate_bounding_box_range(AABoundingBox *boxes, size_t size, AABoundingBox *box_range);
void calculate_centroid_range(Vec3 *centroids, size_t size, Vec3 *centroid);
int argmax(int *array, size_t size);
BVHPreparedData *bvh_prepare_data(TriangleBVH *triangles, size_t size);
void bvh_build_tree(BVHPreparedData *prepared_data, BVHNode *node, int depth);
// void inv_ray_direction();
// void bounding_box_intersection();
// void bvh_traverse_tree();
// void bvh_raycast_bfs();

#endif