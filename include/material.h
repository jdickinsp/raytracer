#ifndef MATERIAL_H
#define MATERIAL_H

#include <scene.h>
#include <shared.h>
#include <vectors.h>

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction);

#endif
