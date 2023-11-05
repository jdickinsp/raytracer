#ifndef OBJECT_MESH_H
#define OBJECT_MESH_H

#include <bvh.h>
#include <material.h>
#include <shared.h>
#include <vectors.h>

MeshInfo *object_mesh_create(Mesh *mesh, Material *material, Vec3 offset);
float object_mesh_intersection(MeshInfo *object_mesh, Ray *ray, HitInfo *hit_info);
void bvh_raycast(MeshInfo *obj_mesh, Ray *ray, BVHitInfo *bv_hit);

void mesh_boundaries(Mesh *mesh, Vec3 *min_b, Vec3 *max_b, Vec3 *center_b);
void mesh_set_face_normals(Mesh *mesh);
BoundingSphere *bounding_sphere_create(Vec3 max_bounds, Vec3 min_bounds, Vec3 center);

#endif
