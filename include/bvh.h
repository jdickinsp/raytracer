#ifndef BVH_H
#define BVH_H

#include <collections.h>
#include <math.h>
#include <stdbool.h>
#include <vectors.h>

#define BVH_MAX_DEPTH 50

static float EPSILON = 1e-5;

typedef struct {
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
} BVTriangle;

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

typedef struct {
    Vec3 origin;
    Vec3 direction;
    float t;
} BVRay;

typedef struct {
    int index;
    float t;
    BVRay *ray;
} BVHitInfo;

static void calculate_bounding_box(const BVTriangle *triangle, AABoundingBox *box);
static void calculate_centroid(const BVTriangle *t, Vec3 *centroid);
static void calculate_centroid_from_box(const AABoundingBox *box, Vec3 *centroid);
static void calculate_bounding_box_range(const Primatives *primatives, int lo, int hi, AABoundingBox *box_range);
static void calculate_centroid_range(const Primatives *primatives, int lo, int hi, Vec3 *centroid);
static int argmax(const float *array, size_t size);
static void bvh_swap_primatives(Primatives *primatives, int a, int b);
static int bvh_partition(Primatives *primatives, int lo, int hi, int axis, float pivot);
Primatives *bvh_prepare_data(const BVTriangle *triangles, size_t size);
BVHNode *bvh_build_child(const Primatives *primatives, int lo, int hi, int depth);
BVHNode *bvh_build_tree(Primatives *primatives);
void bvh_pprint(BVHNode *node);
void bvh_traverse_tree(BVHNode *node);
float inv_ray_direction(float v);
bool bounding_box_intersection(const AABoundingBox *box, BVRay *ray, float *t);
void bvh_raycast_bfs(BVHNode *node, BVRay *ray, BVHitInfo *hit_info);
void find_ray_from_triangle(const Vec3 origin, const BVTriangle *triangle, BVRay *ray);

#endif