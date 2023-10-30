#ifndef RAY_TRACE_H
#define RAY_TRACE_H

#include <scene.h>
#include <shared.h>
#include <stdbool.h>
#include <utils.h>
#include <vectors.h>

void phong_shading(ObjectList *lights, HitInfo *hit_info, Vec3 *attenuation);
Vec3 ray_trace_color(Scene *scene, Ray *ray, int depth);

#endif
