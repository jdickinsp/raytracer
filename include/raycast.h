#ifndef RAYCAST_H
#define RAYCAST_H

#include <camera.h>
#include <constants.h>
#include <material.h>
#include <math.h>
#include <matrix44.h>
#include <scene.h>
#include <stdbool.h>
#include <texture.h>
#include <vectors.h>

bool detect_ray_hits(Ray *ray, ObjectList *objects, HitInfo *hit_info, float near, float far);
Vec3 ray_trace_color(Scene *scene, Ray *ray, int depth);
Vec3 path_trace_color(Scene *scene, Ray *ray, int depth);

#endif
