#ifndef CAMERA_H
#define CAMERA_H

#include <constants.h>
#include <matrix44.h>
#include <scene.h>
#include <vectors.h>

typedef struct {
    int samples_per_pixel;
    int rendering_depth;
    int width;
    int height;
    float aspect_ratio;
    float vfov;
    Vec3 position;
    Vec3 direction;
    Matrix44 transform;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;
    Vec3 pixel_upper_left;
    Vec3 lookfrom;
    Vec3 lookat;
    Vec3 up;
    Vec3 u, v, w;
} Camera;

void camera_init(Camera *camera, int width, int height);
void camera_lookat(Vec3 from, Vec3 to, Vec3 up, Matrix44 *m);
Vec3 camera_pixel_sample(Camera *camera);
void camera_ray_from_pixel(Camera *camera, int i, int j, Ray *ray);

#endif
