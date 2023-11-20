#ifndef SCENE_H
#define SCENE_H

#include <bvh.h>
#include <constants.h>
#include <image.h>
#include <matrix44.h>
#include <shared.h>
#include <stdbool.h>
#include <stdio.h>
#include <texture.h>
#include <vectors.h>

Scene *scene_create(RenderOptions *options, Camera *camera);

ObjectList *object_list_create(void);
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
Triangle *triangle_create_with_normals(Vec3 a, Vec3 b, Vec3 c, Material *material, Vec3 normal1, Vec3 normal2,
                                       Vec3 normal3);
Vec3 triangle_normal_at(Triangle *triangle);
float triangle_intersection(Triangle *triangle, Ray *ray);

Plane *plane_create(Vec3 center, Vec3 normal, Material *material);
Vec3 plane_normal_at(Plane *planeV);
float plane_intersection(Plane *plane, Ray *ray);

Sphere *sphere_create(Vec3 center, float radius, Material *material);
Vec3 sphere_normal_at(Sphere *s, Vec3 p);
float sphere_intersection(Sphere *s, Ray *ray, HitInfo *hit_info);
Vec2 sphere_uv_texture_coord(Vec3 p);

void object_list_free(ObjectList *object_list);
void scene_free(Scene *scene);

#endif
