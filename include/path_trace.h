#ifndef PATH_TRACE_H
#define PATH_TRACE_H

#include <scene.h>
#include <shared.h>
#include <stdbool.h>
#include <vectors.h>

bool material_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool lambertian_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool metal_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool dielectric_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
float reflectance(float cosine, float ref_index);
Vec3 path_trace_color(Scene *scene, Ray *ray, int depth);

#endif
