#include <examples.h>
#include <image.h>
#include <math.h>
#include <matrix44.h>
#include <omp.h>
#include <path_trace.h>
#include <ray_trace.h>
#include <raycast.h>
#include <scene.h>
#include <vectors.h>

Scene *scene_selector(int index, RenderOptions *options);
void raytrace_image(Scene *scene, Image *image);

Scene *scene_selector(int index, RenderOptions *options) {
    Scene *scene;
    switch (index) {
        case 1:
            scene = create_scene_basic(options);
            break;
        case 2:
            scene = create_scene_rand_spheres(options);
            break;
        case 3:
            scene = create_scene_triangle(options);
            break;
        case 4:
            scene = create_scene_box(options);
            break;
        case 5:
            scene = create_scene_with_obj_file(options);
            break;
        case 6:
            scene = create_scene_with_obj_to_mesh(options);
            break;
        case 7:
            scene = create_scene_with_rand_cubes(options);
            break;
        case 8:
            scene = create_scene_with_texture(options);
            break;
        case 9:
            scene = create_scene_with_binary_tree(options);
            break;
        case 10:
            scene = create_scene_with_bvh(options);
            break;
        case 11:
            scene = create_scene_with_bvh_from_obj(options);
            break;
        default:
            exit(1);
    }
    return scene;
}

void raytrace_image(Scene *scene, Image *image) {
    Camera *camera = scene->camera;
    if (camera == NULL) {
        fprintf(stderr, "Error: must include a camera.\n");
        abort();
    }
    int buffer_size = (image->width * image->height);
    Vec3 *frame_buffer = malloc(sizeof(Vec3) * buffer_size);
    // find all the rays that intersect the camera plane
    int total_progress = (image->height) / 25;
    int k = 0;
    printf("progress: ");
#pragma omp parallel for schedule(dynamic, 1)
    for (int j = 0; j < image->height; j++) {
        for (int i = 0; i < image->width; i++) {
            Ray ray;
            Vec3 pixel_color = {0, 0, 0};
            Vec3 p;
            for (int sample = 0; sample < camera->samples_per_pixel; sample++) {
                camera_ray_from_subpixel(camera, i, j, &ray);
                switch (scene->render_options->rendering_type) {
                    case RAY_TRACE:
                        p = ray_trace_color(scene, &ray, camera->rendering_depth);
                        break;
                    case PATH_TRACE:
                        p = path_trace_color(scene, &ray, camera->rendering_depth);
                        break;
                    default:
                        fprintf(stderr, "Not a valid rendering_type");
                        abort();
                }
                pixel_color = vec3_add(pixel_color, p);
            }
            // #pragma omp critical
            frame_buffer[j * image->width + i] = pixel_color;
        }
        // #pragma omp atomic
        ++k;
        if (k % total_progress == 0) {
            printf("#");
        }
    }
    printf("\n");
    image_from_buffer(image, frame_buffer, buffer_size, camera->samples_per_pixel);
    free(frame_buffer);
}

int main() {
    printf("raytracer\n");
    // (1366, 768) (1920, 1080)
    RenderOptions options = {
        .rendering_type = RAY_TRACE, .width = 1920, .height = 1080, .samples_per_pixel = 5, .rendering_depth = 5};
    Scene *scene = scene_selector(6, &options);
    Image *image = image_create(options.width, options.height);
    raytrace_image(scene, image);
    image_save_png(image);
    image_free(image);
    free(scene);
    return 0;
}
