#ifndef MATERIAL_H
#define MATERIAL_H

#include <scene.h>
#include <shared.h>
#include <stdbool.h>
#include <vectors.h>

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction);
bool material_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool lambertian_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool metal_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
bool dielectric_scatter(Material *mat, Ray *ray, HitInfo *hit_info, Vec3 *attenuation, Ray *scattered);
float reflectance(float cosine, float ref_index);

#endif
