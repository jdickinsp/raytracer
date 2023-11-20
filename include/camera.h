#ifndef CAMERA_H
#define CAMERA_H

#include <constants.h>
#include <matrix44.h>
#include <scene.h>
#include <shared.h>
#include <vectors.h>

void camera_init(Camera *camera, RenderOptions *render_options, Vec3 *lookfrom, Vec3 *lookat);
void camera_lookat(Vec3 from, Vec3 to, Vec3 up, Matrix44 *m);
Vec3 camera_pixel_sample(Camera *camera);
void camera_ray_from_subpixel(Camera *camera, int i, int j, Ray *ray);
void camera_ray_from_pixel(Camera *camera, int i, int j, Ray *ray);

#endif
