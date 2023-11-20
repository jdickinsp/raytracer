#include <material.h>

Material *material_create(float albedo, float Kd, float Ks, bool reflective, Vec3 color, float index_of_refraction) {
    Material *material = malloc(sizeof(Material));
    material->albedo = albedo;
    material->Kd = Kd;
    material->Ks = Ks;
    material->reflective = reflective;
    material->color = color;
    material->index_of_refraction = index_of_refraction;
    material->checkerboard = false;
    material->texture = NULL;
    material->scale = 0.1f;
    return material;
}
