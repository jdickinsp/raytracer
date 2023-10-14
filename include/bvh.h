#ifndef BVH_H
#define BVH_H

#include <collections.h>
#include <math.h>
#include <vectors.h>

#define BVH_MAX_DEPTH 4

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
    AABoundingBox box;
    Vec3 centroid;
    int index;
} PrimativeInfo;

typedef struct {
    PrimativeInfo *array;
    size_t size;
} Primatives;

void calculate_bounding_box(TriangleBVH *triangle, AABoundingBox *box);
void calculate_centroid(TriangleBVH *t, Vec3 *centroid);
void calculate_centroid_from_box(AABoundingBox *box, Vec3 *centroid);
void calculate_bounding_box_range(AABoundingBox *boxes, size_t size, AABoundingBox *box_range);
void calculate_centroid_range(Vec3 *centroids, size_t size, Vec3 *centroid);
int argmax(int *array, size_t size);
Primatives *bvh_prepare_data(TriangleBVH *triangles, size_t size);
void bvh_build_tree(Primatives *primatives);
// void inv_ray_direction();
// void bounding_box_intersection();
// void bvh_traverse_tree();
// void bvh_raycast_bfs();

#endif