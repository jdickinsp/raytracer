#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdbool.h>
#include <math.h>
#include <camera.h>
#include <constants.h>
#include <matrix44.h>
#include <vectors.h>
#include <scene.h>
#include <texture.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far);
Vec3 cast_ray(Ray *ray, RenderingOptions *options, Scene *scene);
Vec3 raycast_color(Ray *ray, RenderingOptions *options, Scene *scene, int depth);

#endif