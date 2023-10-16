#include <camera.h>

void camera_init(Camera *camera, int width, int height) {
    camera->samples_per_pixel = 10;
    camera->rendering_depth = 25;
    camera->width = width;
    camera->height = height;
    camera->aspect_ratio = (float)width / (float)height;
    camera->vfov = 50.f;
    camera->lookfrom = vec3_create(-1.6f, -0.4f, 4.0f);
    camera->lookat = vec3_create(0, 0, -1);
    camera->up = vec3_create(0, 1, 0);
    camera->position = camera->lookfrom;

    float focal_length = vec3_mag(vec3_sub(camera->lookfrom, camera->lookat));
    float theta = camera->vfov * (M_PI / 180.f);
    float h = tanf(theta / 2.0f);
    float viewport_height = 2.0f * h * focal_length;
    float viewport_width = viewport_height * camera->aspect_ratio;

    camera->w = vec3_norm(vec3_sub(camera->lookfrom, camera->lookat));
    camera->u = vec3_norm(cross_product(camera->up, camera->w));
    camera->v = cross_product(camera->w, camera->u);

    Vec3 viewport_u = vec3_mul(camera->u, viewport_width);
    Vec3 viewport_v = vec3_mul(vec3_neg(camera->v), viewport_height);

    camera->pixel_delta_u = vec3_mul(viewport_u, (1.0f / (float)width));
    camera->pixel_delta_v = vec3_mul(viewport_v, (1.0f / (float)height));

    Vec3 upper_left_corner = vec3_sub(vec3_sub(camera->position, vec3_mul(camera->w, focal_length)),
                                      vec3_sub(vec3_mul(viewport_u, 0.5f), vec3_mul(viewport_v, 0.5f)));
    Vec3 pixel_upper_left =
        vec3_add(upper_left_corner, vec3_mul(vec3_sub(camera->pixel_delta_u, camera->pixel_delta_v), 0.5f));
    camera->pixel_upper_left = pixel_upper_left;
}

void camera_lookat(Vec3 eye, Vec3 center, Vec3 up, Matrix44 *m) {
    Vec3 forward = vec3_norm(vec3_sub(eye, center));
    Vec3 right = vec3_norm(cross_product(up, forward));
    Vec3 new_up = cross_product(forward, right);
    m->m11 = right.x;
    m->m12 = right.y;
    m->m13 = right.z;
    m->m14 = 0;
    m->m21 = new_up.x;
    m->m22 = new_up.y;
    m->m23 = new_up.z;
    m->m24 = 0;
    m->m31 = forward.x;
    m->m32 = forward.y;
    m->m33 = forward.z;
    m->m34 = 0;
    m->m41 = eye.x;
    m->m42 = eye.y;
    m->m43 = eye.z;
    m->m44 = 1;
}

Vec3 camera_pixel_sample(Camera *camera) {
    float px = -0.5f + rand_range(0, 1);
    float py = -0.5f + rand_range(0, 1);
    Vec3 sample = vec3_add(vec3_mul(camera->pixel_delta_u, px), vec3_mul(camera->pixel_delta_v, py));
    return sample;
}

void camera_ray_from_pixel(Camera *camera, int i, int j, Ray *ray) {
    Vec3 pixel_center = vec3_add(camera->pixel_upper_left, vec3_sub(vec3_mul(camera->pixel_delta_u, (float)i),
                                                                    vec3_mul(camera->pixel_delta_v, (float)j)));
    Vec3 pixel_sample = vec3_add(pixel_center, camera_pixel_sample(camera));
    ray->origin = camera->position;
    ray->direction = vec3_norm(vec3_sub(pixel_sample, ray->origin));
}
