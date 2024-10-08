#ifndef IMAGE_H
#define IMAGE_H

#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <vectors.h>

int image_save_png(Image *image);
int image_save_ppm(Image *image);
Image *image_create(int width, int height);
void image_set_pixel(Image *image, int i, int j, unsigned char r, unsigned char g, unsigned char b);
void image_free(Image *image);
void image_from_buffer(Image *image, Vec3 *frame_buffer, int buffer_size, int samples_per_pixel);

TextureImage *texture_load(const char *filename);
Vec3 texture_pixel_data(TextureImage *texture, int i, int j);

#endif
