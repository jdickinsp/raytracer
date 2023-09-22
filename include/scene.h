#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>
#include <stdio.h>
#include <constants.h>
#include <matrix44.h>
#include <vectors.h>
#include <image.h>
#include <texture.h>

typedef struct {
    float albedo;
    float Kd;
    float Ks;
    // bool diffuse;
    bool reflective;
    Vec3 color;
    float index_of_refraction;
    bool checkerboard;
    TextureImage *texture;
} Material;

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
    float radius;
    Material *material;
} Sphere;

typedef struct {
    Vec3 min_bounds;
    Vec3 max_bounds;
    Sphere sphere;
} BoundingSphere;

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
    Material *material;
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
    int count;
} ObjectList;

typedef struct Scene {
    struct ObjectList *objects;
    struct ObjectList *lights;
    Matrix44 world_matrix;
} Scene;

typedef struct {
    float hit;
    ObjectNode *node;
    Vec3 position;
    Vec3 normal;
    Vec3 color;
    float u, v;
    bool front_face;
    Material *material;
} HitInfo;

typedef struct {
    Vec3 origin;
    Vec3 direction;
    float t;
} Ray;

typedef enum {
    RENDER_BASIC = 0,
    RENDER_ADVANCED = 1,
    RENDER_SHADOW = 2
} RenderingType;

typedef struct {
    RenderingType rendering_type; 
} RenderingOptions;

ObjectList *object_list_create();
void object_list_add(ObjectList *object_list, Object *object, ObjectType type);
float object_intersection(Object *object, ObjectType type, Ray *ray, HitInfo *hit_info);
Vec3 object_normal_at(Object *object, ObjectType type, Vec3 p);
Vec3 object_color(Object *object, ObjectType type);
Material *object_material(Object *object, ObjectType type);

Vec3 ray_normal_at(Ray *ray, float t);

DirectionalLight *directional_light_create(Vec3 direction, Vec3 color, float intensity);
PointLight *point_light_create(Vec3 position, Vec3 color, float intensity);
Vec3 light_direction(Object *object, ObjectType type, Vec3 point);
float light_intensity(Object *object, ObjectType type);

Triangle *triangle_create(Vec3 a, Vec3 b, Vec3 c, Material *material);
Triangle *triangle_create_with_normals(Vec3 a, Vec3 b, Vec3 c, Material *material, 
        Vec3 normal1, Vec3 normal2, Vec3 normal3);
Vec3 triangle_normal_at(Triangle *triangle);
float triangle_intersection(Triangle *triangle, Ray *ray);

Plane *plane_create(Vec3 center, Vec3 normal, Material *material);
Vec3 plane_normal_at(Plane *planeV);
float plane_intersection(Plane *plane, Ray *ray);

Sphere *sphere_create(Vec3 center, float radius, Material *material);
Vec3 sphere_normal_at(Sphere *s, Vec3 p);
float sphere_intersection(Sphere *s, Ray *ray, HitInfo *hit_info);
Vec2 sphere_uv_texture_coord(Vec3 p);

ObjectMesh *object_mesh_create(Mesh *mesh, Material *material);
float object_mesh_intersection(ObjectMesh *object_mesh, Ray *ray, HitInfo *hit_info);
Vec3 mesh_triangle_normal_at(Vec3 a, Vec3 b, Vec3 c);
void mesh_boundaries(Mesh *mesh, Vec3 *min_b, Vec3 *max_b, Vec3 *center_b);
float mesh_triangle_intersection(Ray *ray, Vec3 a, Vec3 b, Vec3 c, Vec3 *uvw);

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction);

#endif