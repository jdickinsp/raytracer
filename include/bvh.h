#ifndef BVH_H
#define BVH_H

#include <vectors.h>

typedef struct {
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
} Triangle;

typedef struct AABoundingBox {
    Vec3 min;
    Vec3 max;
} AABoundingBox;

typedef struct BVHLeft {
    int data;
    AABoundingBox *aabb;
} BVHLeft;

struct BVHNode {
    AABoundingBox *aabb;
    struct BVHNode *left;
    struct BVHNode *right;
} BVHNode;

typedef struct {
    AABoundingBox *boxes;
    Vec3 *centroids;
    int indexes;
    int size;
} BVHPreparedData;

void calculate_bounding_boxes(Triangle *triangle);
void calculate_centroid(Triangle *triangle);
void calculate_centroid_from_box(AABoundingBox *box);
void calculate_bounding_box_range(AABoundingBox *boxes);
void inv_ray_direction();
void bounding_box_intersection();

void bvh_prepare_data();
void bvh_build_tree();
void bvh_traverse_tree();
void bvh_raycast_bfs();

#endif