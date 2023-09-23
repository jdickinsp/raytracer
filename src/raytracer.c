#include <examples.h>
#include <image.h>
#include <math.h>
#include <matrix44.h>
#include <raycast.h>
#include <scene.h>
#include <vectors.h>

Scene *scene_selector(int index) {
    Scene *scene;
    switch (index) {
        case 1:
            scene = create_scene_basic();
            break;
        case 2:
            scene = create_scene_rand_spheres();
            break;
        case 3:
            scene = create_scene_triangle();
            break;
        case 4:
            scene = create_scene_box();
            break;
        case 5:
            scene = create_scene_with_obj_file();
            break;
        case 6:
            scene = create_scene_with_obj_to_mesh();
            break;
        case 7:
            scene = create_scene_with_texture();
            break;
        default:
            exit(1);
    }
    return scene;
}

void raytrace_image(Image *image) {
    Scene *scene = scene_selector(1);
    RenderingOptions options = {.rendering_type = RENDER_SHADOW};
    Camera camera;
    camera_init(&camera, image->width, image->height);

    int buffer_size = image->width * image->height;
    Vec3 *frame_buffer = malloc(buffer_size * sizeof(Vec3));
    // find all the rays that intersect the camera plane
    int total_progress = (image->height * image->width) / 25;
    int k = 0;
    printf("progress: ");
    Ray ray;
    for (int j = 0; j < image->height; j++) {
        for (int i = 0; i < image->width; i++) {
            Vec3 pixel_color = {0, 0, 0};
            for (int sample = 0; sample < camera.samples_per_pixel; ++sample) {
                camera_ray_from_pixel(&camera, i, j, &ray);
                Vec3 p = raycast_color(&ray, &options, scene, camera.rendering_depth);
                pixel_color = vec3_add(pixel_color, p);
            }
            frame_buffer[j * image->width + i] = pixel_color;
            if (k % total_progress == 0) {
                printf("#");
            }
            ++k;
        }
    }
    printf("\n");
    image_from_buffer(image, frame_buffer, buffer_size, camera.samples_per_pixel);
}

int main() {
    printf("raytracer\n");
    // Image *image = image_create(1366, 768);
    Image *image = image_create(768, 512);
    raytrace_image(image);
    // image_save_ppm(image);
    image_save_png(image);
    image_close(image);
    return 0;
}
