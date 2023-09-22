#include <image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int image_save_png(Image *image) {
    stbi_write_png("./outputs/image.png", image->width, image->height, 3, image->data, sizeof(Pixel) * image->width);
    return 0;
}

int image_save_ppm(Image *image) {
    printf("save file (%ix%i)\n", image->width, image->height);
    FILE *fp = fopen("./outputs/image.ppm", "wb");
    if (!fp) {
        fprintf(stderr, "Unable to open file\n");
        exit(1);
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "# comment\n");
    fprintf(fp, "%d %d\n", image->width, image->height);
    fprintf(fp, "255\n");
    fwrite(image->data, sizeof(Pixel), image->width * image->height, fp);
    fclose(fp);
    return 0;
}

Image *image_create(int width, int height) {
    Image *image = malloc(sizeof(Image));
    image->width = width;
    image->height = height;
    image->data = malloc(image->width * image->height * sizeof(Pixel));
    // initialize colors to zero
    for (int j=0; j < image->height; j++) {
        for (int i=0; i < image->width; i++) {
            unsigned char c = 0;
            image_set_pixel(image, i, j, c, c, c);
        }
    }
    return image;
}

void image_close(Image *image) {
    free(image->data);
    free(image);
}

void image_set_pixel(Image *image, int i, int j, unsigned char r, unsigned char g, unsigned char b) {
    image->data[j * image->width + i].r = r;
    image->data[j * image->width + i].g = g;
    image->data[j * image->width + i].b = b;
}

void image_from_buffer(Image *image, Vec3 *frame_buffer, int buffer_size, int samples_per_pixel) {
    for (int j=0; j < image->height; j++) {
        for (int i=0; i < image->width; i++) {
            Vec3 p = frame_buffer[j * image->width + i];
            unsigned char red = clamp_range(sqrt(p.x / samples_per_pixel), 0.000, .999) * 256;
            unsigned char green = clamp_range(sqrt(p.y / samples_per_pixel), 0.000, .999) * 256;
            unsigned char blue = clamp_range(sqrt(p.z / samples_per_pixel), 0.000, .999) * 256;
            image_set_pixel(image, i, j, red, green, blue);
        }
    }
}

TextureImage *texture_load(const char *filename) {
    int width, height;
    int bytes_per_pixel = 3;
    int n = bytes_per_pixel;
    stbi_uc *data = stbi_load(filename, &width, &height, &n, bytes_per_pixel);
    TextureImage *texture = malloc(sizeof(TextureImage*));
    texture->width = width;
    texture->height = height;
    texture->bytes_per_pixel = bytes_per_pixel;
    texture->data = data;

    return texture;
}

int texture_clamp(int x, int low, int high) {
    if (x < low) return low;
    if (x < high) return x;
    return x - 1;
}

Vec3 texture_pixel_data(TextureImage *texture, int i, int j) {
    int bytes_per_scanline = texture->width * texture->bytes_per_pixel;
    i = texture_clamp(i, 0, texture->width);
    j = texture_clamp(j, 0, texture->height);
    const unsigned char *data = texture->data + j * bytes_per_scanline + i * texture->bytes_per_pixel;
    int x = (int)data[0];
    int y = (int)data[1];
    int z = (int)data[2];
    Vec3 pixel = { x / 255.f, y / 255.f, z / 255.f };
    return pixel;
}
