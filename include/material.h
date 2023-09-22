#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>
#include <scene.h>
#include <vectors.h>

bool material_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool lambertian_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool metal_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool dielectric_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
float reflectance(float cosine, float ref_index);

#endif
