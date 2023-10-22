#ifndef SHARED_H
#define SHARED_H

#include <vectors.h>

typedef struct {
    float albedo;
    float Kd;
    float Ks;
    // bool diffuse;
    bool reflective;
    Vec3 color;
    float index_of_refraction;
    bool checkerboard;
    float scale;
    TextureImage *texture;
} Material;

typedef struct {
    Vec3 position;
    float radius;
    Material *material;
} Sphere;

typedef struct {
    Vec3 min_bounds;
    Vec3 max_bounds;
    Sphere sphere;
} BoundingSphere;

typedef enum {
    SphereType = 0,
    PlaneType = 1,
    TriangleType = 2,
    ObjectMeshType = 3,
    DirectionalLightType = 4,
    PointLightType = 5
} ObjectType;

typedef struct {
    Vec3 position;
    Vec3 normal;
    Material *material;
} Plane;

typedef struct {
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 normal;
    Vec3 normal1;
    Vec3 normal2;
    Vec3 normal3;
    float d;
    Material *material;
} Triangle;

typedef struct {
    int face_count;
    int vertex_count;
    int *face_index;
    int *vertex_index;
    Vec3 *vertices;
    Vec3 *vertex_normals;
    Vec2 *texture_uv;
    Vec3 *face_normals;
    float *directions;
} Mesh;

typedef struct {
    Mesh *mesh;
    BoundingSphere *bounding_sphere;
    BVHNode *bvh;
    Primatives *primatives;
    Material *material;
    Vec3 offset;
} ObjectMesh;

typedef struct {
    Vec3 color;
    Vec3 direction;
    float intensity;
} DirectionalLight;

typedef struct {
    Vec3 position;
    Vec3 color;
    float intensity;
} PointLight;

// TODO: change to use void *data
typedef union {
    Sphere sphere;
    Plane plane;
    Triangle triangle;
    ObjectMesh object_mesh;
    DirectionalLight directional_light;
    PointLight point_light;
} Object;

typedef struct ObjectNode {
    Object *current;
    ObjectType type;
    struct ObjectNode *next;
} ObjectNode;

typedef struct ObjectList {
    struct ObjectNode *head;
    int size;
} ObjectList;

typedef struct {
    int samples_per_pixel;
    int rendering_depth;
    int width;
    int height;
    float aspect_ratio;
    float vfov;
    Vec3 position;
    Vec3 direction;
    Matrix44 transform;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;
    Vec3 pixel_upper_left;
    Vec3 lookfrom;
    Vec3 lookat;
    Vec3 up;
    Vec3 u, v, w;
} Camera;

typedef struct {
    float hit;
    ObjectNode *node;
    Vec3 position;
    Vec3 normal;
    Vec3 color;
    float u, v;
    bool front_face;
    Material *material;
    int tri_index;
    float iu, iv;
    Ray *ray;
} HitInfo;

typedef struct {
    int index;
    Ray *ray;
    bool has_hit;
    float hit;
    Vec3 barycentric;
} BVHitInfo;

typedef enum { WHITTED_RAY_TRACING = 0, PATH_TRACING = 1, RAY_TRACING_2 = 2, RAY_TRACING_3 = 3 } RenderingType;

typedef struct {
    RenderingType rendering_type;
    int width;
    int height;
    int samples_per_pixel;
    int rendering_depth;
} RenderOptions;

typedef struct {
    ObjectList *objects;
    ObjectList *lights;
    Camera *camera;
    RenderOptions *render_options;
    Matrix44 world_matrix;
} Scene;

#endif