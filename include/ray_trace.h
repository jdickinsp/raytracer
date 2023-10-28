#ifndef RAY_TRACE_H
#define RAY_TRACE_H

#include <scene.h>
#include <shared.h>
#include <stdbool.h>
#include <vectors.h>

// whitted ray tracing
bool phong_shading(ObjectList *lights, HitInfo *hit_info, Vec3 *attenuation);
Vec3 ray_trace_color(Scene *scene, Ray *ray, int depth);

#endif
