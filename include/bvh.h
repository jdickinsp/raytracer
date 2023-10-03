#ifndef BVH_H
#define BVH_H

#include <vectors.h>

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

#endif