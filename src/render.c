#include <stdio.h>
#include <stdlib.h>

#include "examples.h"
#include "image.h"
#include "math.h"
#include "matrix44.h"
#include "omp.h"
#include "path_trace.h"
#include "ray_trace.h"
#include "raycast.h"
#include "scene.h"
#include "vectors.h"

/**
 * @brief Select a scene based on the provided index and render options.
 *
 * @param index The index of the scene to select.
 * @param options The render options.
 * @return Scene* The selected scene.
 */
Scene *select_scene(int index, RenderOptions *options) {
    Scene *scene = NULL;
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
            fprintf(stderr, "Invalid scene index: %d\n", index);
            exit(1);
    }
    return scene;
}

/**
 * @brief Render the image using the provided scene.
 *
 * @param scene The scene to render.
 * @param image The image to store the rendered result.
 */
void render_image(Scene *scene, Image *image) {
    Camera *camera = scene->camera;
    if (camera == NULL) {
        fprintf(stderr, "Error: Camera not found in the scene.\n");
        return;
    }

    int buffer_size = image->width * image->height;
    Vec3 *frame_buffer = (Vec3 *)malloc(sizeof(Vec3) * buffer_size);
    if (frame_buffer == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for frame buffer.\n");
        return;
    }

    int total_progress = image->height / 50;
    int progress = 0;

    printf("Rendering progress: [");
    fflush(stdout);

#pragma omp parallel for schedule(dynamic, 1)
    for (int j = 0; j < image->height; j++) {
        for (int i = 0; i < image->width; i++) {
            Ray ray;
            Vec3 pixel_color = {0, 0, 0};

            for (int sample = 0; sample < camera->samples_per_pixel; sample++) {
                camera_ray_from_subpixel(camera, i, j, &ray);

                Vec3 color;
                switch (scene->render_options->rendering_type) {
                    case RAY_TRACE:
                        color = ray_trace_color(scene, &ray, camera->rendering_depth);
                        break;
                    case PATH_TRACE:
                        color = path_trace_color(scene, &ray, camera->rendering_depth);
                        break;
                    default:
                        fprintf(stderr, "Invalid rendering type.\n");
                        free(frame_buffer);
                        exit(1);
                }

                pixel_color = vec3_add(pixel_color, color);
            }

            frame_buffer[j * image->width + i] = pixel_color;
        }

#pragma omp critical
        {
            progress++;
            if (progress % total_progress == 0) {
                printf("#");
                fflush(stdout);
            }
        }
    }

    printf("]\n");

    image_from_buffer(image, frame_buffer, buffer_size, camera->samples_per_pixel);
    free(frame_buffer);
}

/**
 * @brief Main function.
 *
 * @return int Exit status.
 */
int main() {
    printf("Raytracer\n");

    RenderOptions options = {
        .rendering_type = RAY_TRACE, .width = 1920, .height = 1080, .samples_per_pixel = 5, .rendering_depth = 5};

    Scene *scene = select_scene(6, &options);
    if (scene == NULL) {
        fprintf(stderr, "Failed to create scene.\n");
        return 1;
    }

    Image *image = image_create(options.width, options.height);
    if (image == NULL) {
        fprintf(stderr, "Failed to create image.\n");
        free(scene);
        return 1;
    }

    render_image(scene, image);

    if (image_save_png(image) != 0) {
        fprintf(stderr, "Failed to save image.\n");
    }

    image_free(image);
    free(scene);

    return 0;
}
