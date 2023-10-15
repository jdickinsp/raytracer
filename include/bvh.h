#ifndef BVH_H
#define BVH_H

#include <collections.h>
#include <math.h>
#include <stdbool.h>
#include <vectors.h>

#define BVH_MAX_DEPTH 250

static float EPSILON = 1e-5;

typedef struct {
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
} TriangleBVH;

typedef struct AABoundingBox {
    Vec3 min;
    Vec3 max;
} AABoundingBox;

typedef struct BVHNode {
    AABoundingBox *aabb;
    int data;
    bool is_leaf;
    struct BVHNode *left;
    struct BVHNode *right;
} BVHNode;

typedef struct {
    AABoundingBox aabb;
    Vec3 centroid;
    int index;
} PrimativeInfo;

typedef struct {
    PrimativeInfo *array;
    size_t size;
} Primatives;

static void calculate_bounding_box(TriangleBVH *triangle, AABoundingBox *box);
static void calculate_centroid(TriangleBVH *t, Vec3 *centroid);
static void calculate_centroid_from_box(AABoundingBox *box, Vec3 *centroid);
static void calculate_bounding_box_range(Primatives *primatives, int lo, int hi, AABoundingBox *box_range);
static void calculate_centroid_range(Primatives *primatives, int lo, int hi, Vec3 *centroid);
static int argmax(float *array, size_t size);
static int bvh_partition(Primatives *primatives, int lo, int hi, int axis, float pivot);
Primatives *bvh_prepare_data(TriangleBVH *triangles, size_t size);
BVHNode *bvh_build_child(Primatives *primatives, int lo, int hi, int depth);
BVHNode *bvh_build_tree(Primatives *primatives);
void bvh_pprint(BVHNode *node);
void bvh_traverse_tree(BVHNode *node);
// void inv_ray_direction();
// void bounding_box_intersection();
// void bvh_raycast_bfs();

#endif